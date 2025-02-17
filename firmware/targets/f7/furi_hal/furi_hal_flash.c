#include <furi_hal_flash.h>
#include <furi_hal_bt.h>
#include <furi.h>
#include <ble.h>
#include <shci.h>

#include <stm32wbxx.h>

#define FURI_HAL_TAG "FuriHalFlash"
#define FURI_HAL_CRITICAL_MSG "Critical flash operation fail"
#define FURI_HAL_FLASH_READ_BLOCK 8
#define FURI_HAL_FLASH_WRITE_BLOCK 8
#define FURI_HAL_FLASH_PAGE_SIZE 4096
#define FURI_HAL_FLASH_CYCLES_COUNT 10000
#define FURI_HAL_FLASH_TIMEOUT 1000
#define FURI_HAL_FLASH_KEY1 0x45670123U
#define FURI_HAL_FLASH_KEY2 0xCDEF89ABU
#define FURI_HAL_FLASH_SR_ERRORS                                                               \
    (FLASH_SR_OPERR | FLASH_SR_PROGERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_SIZERR | \
     FLASH_SR_PGSERR | FLASH_SR_MISERR | FLASH_SR_FASTERR | FLASH_SR_RDERR | FLASH_SR_OPTVERR)

#define IS_ADDR_ALIGNED_64BITS(__VALUE__) (((__VALUE__)&0x7U) == (0x00UL))
#define IS_FLASH_PROGRAM_ADDRESS(__VALUE__)                                             \
    (((__VALUE__) >= FLASH_BASE) && ((__VALUE__) <= (FLASH_BASE + FLASH_SIZE - 8UL)) && \
     (((__VALUE__) % 8UL) == 0UL))

/* Free flash space borders, exported by linker */
extern const void __free_flash_start__;

size_t furi_hal_flash_get_base() {
    return FLASH_BASE;
}

size_t furi_hal_flash_get_read_block_size() {
    return FURI_HAL_FLASH_READ_BLOCK;
}

size_t furi_hal_flash_get_write_block_size() {
    return FURI_HAL_FLASH_WRITE_BLOCK;
}

size_t furi_hal_flash_get_page_size() {
    return FURI_HAL_FLASH_PAGE_SIZE;
}

size_t furi_hal_flash_get_cycles_count() {
    return FURI_HAL_FLASH_CYCLES_COUNT;
}

const void* furi_hal_flash_get_free_start_address() {
    return &__free_flash_start__;
}

const void* furi_hal_flash_get_free_end_address() {
    uint32_t sfr_reg_val = READ_REG(FLASH->SFR);
    uint32_t sfsa = (READ_BIT(sfr_reg_val, FLASH_SFR_SFSA) >> FLASH_SFR_SFSA_Pos);
    return (const void*)((sfsa * FURI_HAL_FLASH_PAGE_SIZE) + FLASH_BASE);
}

size_t furi_hal_flash_get_free_page_start_address() {
    size_t start = (size_t)furi_hal_flash_get_free_start_address();
    size_t page_start = start - start % FURI_HAL_FLASH_PAGE_SIZE;
    if(page_start != start) {
        page_start += FURI_HAL_FLASH_PAGE_SIZE;
    }
    return page_start;
}

size_t furi_hal_flash_get_free_page_count() {
    size_t end = (size_t)furi_hal_flash_get_free_end_address();
    size_t page_start = (size_t)furi_hal_flash_get_free_page_start_address();
    return (end - page_start) / FURI_HAL_FLASH_PAGE_SIZE;
}

static void furi_hal_flash_unlock() {
    /* verify Flash is locked */
    furi_check(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U);

    /* Authorize the FLASH Registers access */
    WRITE_REG(FLASH->KEYR, FURI_HAL_FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FURI_HAL_FLASH_KEY2);

    /* verify Flash is unlock */
    furi_check(READ_BIT(FLASH->CR, FLASH_CR_LOCK) == 0U);
}

static void furi_hal_flash_lock(void) {
    /* verify Flash is unlocked */
    furi_check(READ_BIT(FLASH->CR, FLASH_CR_LOCK) == 0U);

    /* Set the LOCK Bit to lock the FLASH Registers access */
    /* @Note  The lock and unlock procedure is done only using CR registers even from CPU2 */
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);

    /* verify Flash is locked */
    furi_check(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U);
}

static void furi_hal_flash_begin_with_core2(bool erase_flag) {
    // Take flash controller ownership
    while(LL_HSEM_1StepLock(HSEM, CFG_HW_FLASH_SEMID) != 0) {
        osThreadYield();
    }

    // Unlock flash operation
    furi_hal_flash_unlock();

    // Erase activity notification
    if(erase_flag) SHCI_C2_FLASH_EraseActivity(ERASE_ACTIVITY_ON);

    // 64mHz 5us core2 flag protection
    for(volatile uint32_t i = 0; i < 35; i++)
        ;

    while(true) {
        // Wait till flash controller become usable
        while(LL_FLASH_IsActiveFlag_OperationSuspended()) {
            osThreadYield();
        };

        // Just a little more love
        taskENTER_CRITICAL();

        // Actually we already have mutex for it, but specification is specification
        if(LL_HSEM_IsSemaphoreLocked(HSEM, CFG_HW_BLOCK_FLASH_REQ_BY_CPU1_SEMID)) {
            taskEXIT_CRITICAL();
            osThreadYield();
            continue;
        }

        // Take sempahopre and prevent core2 from anything funky
        if(LL_HSEM_1StepLock(HSEM, CFG_HW_BLOCK_FLASH_REQ_BY_CPU2_SEMID) != 0) {
            taskEXIT_CRITICAL();
            osThreadYield();
            continue;
        }

        break;
    }
}

static void furi_hal_flash_begin(bool erase_flag) {
    // Acquire dangerous ops mutex
    furi_hal_bt_lock_core2();

    // If Core2 is running use IPC locking
    if(furi_hal_bt_is_alive()) {
        furi_hal_flash_begin_with_core2(erase_flag);
    } else {
        furi_hal_flash_unlock();
    }
}

static void furi_hal_flash_end_with_core2(bool erase_flag) {
    // Funky ops are ok at this point
    LL_HSEM_ReleaseLock(HSEM, CFG_HW_BLOCK_FLASH_REQ_BY_CPU2_SEMID, 0);

    // Task switching is ok
    taskEXIT_CRITICAL();

    // Doesn't make much sense, does it?
    while(READ_BIT(FLASH->SR, FLASH_SR_BSY)) {
        osThreadYield();
    }

    // Erase activity over, core2 can continue
    if(erase_flag) SHCI_C2_FLASH_EraseActivity(ERASE_ACTIVITY_OFF);

    // Lock flash controller
    furi_hal_flash_lock();

    // Release flash controller ownership
    LL_HSEM_ReleaseLock(HSEM, CFG_HW_FLASH_SEMID, 0);
}

static void furi_hal_flash_end(bool erase_flag) {
    // If Core2 is running use IPC locking
    if(furi_hal_bt_is_alive()) {
        furi_hal_flash_end_with_core2(erase_flag);
    } else {
        furi_hal_flash_lock();
    }

    // Release dangerous ops mutex
    furi_hal_bt_unlock_core2();
}

static void furi_hal_flush_cache(void) {
    /* Flush instruction cache  */
    if(READ_BIT(FLASH->ACR, FLASH_ACR_ICEN) == FLASH_ACR_ICEN) {
        /* Disable instruction cache  */
        LL_FLASH_DisableInstCache();
        /* Reset instruction cache */
        LL_FLASH_EnableInstCacheReset();
        LL_FLASH_DisableInstCacheReset();
        /* Enable instruction cache */
        LL_FLASH_EnableInstCache();
    }

    /* Flush data cache */
    if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) == FLASH_ACR_DCEN) {
        /* Disable data cache  */
        LL_FLASH_DisableDataCache();
        /* Reset data cache */
        LL_FLASH_EnableDataCacheReset();
        LL_FLASH_DisableDataCacheReset();
        /* Enable data cache */
        LL_FLASH_EnableDataCache();
    }
}

bool furi_hal_flash_wait_last_operation(uint32_t timeout) {
    uint32_t error = 0;
    uint32_t countdown = 0;

    // Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
    // Even if the FLASH operation fails, the BUSY flag will be reset and an error
    // flag will be set
    countdown = timeout;
    while(READ_BIT(FLASH->SR, FLASH_SR_BSY)) {
        if(LL_SYSTICK_IsActiveCounterFlag()) {
            countdown--;
        }
        if(countdown == 0) {
            return false;
        }
    }

    /* Check FLASH operation error flags */
    error = FLASH->SR;

    /* Check FLASH End of Operation flag */
    if((error & FLASH_SR_EOP) != 0U) {
        /* Clear FLASH End of Operation pending bit */
        CLEAR_BIT(FLASH->SR, FLASH_SR_EOP);
    }

    /* Now update error variable to only error value */
    error &= FURI_HAL_FLASH_SR_ERRORS;

    furi_check(error == 0);

    /* clear error flags */
    CLEAR_BIT(FLASH->SR, error);

    /* Wait for control register to be written */
    countdown = timeout;
    while(READ_BIT(FLASH->SR, FLASH_SR_CFGBSY)) {
        if(LL_SYSTICK_IsActiveCounterFlag()) {
            countdown--;
        }
        if(countdown == 0) {
            return false;
        }
    }
    return true;
}

bool furi_hal_flash_erase(uint8_t page) {
    furi_hal_flash_begin(true);

    // Ensure that controller state is valid
    furi_check(FLASH->SR == 0);

    /* Verify that next operation can be proceed */
    furi_check(furi_hal_flash_wait_last_operation(FURI_HAL_FLASH_TIMEOUT));

    /* Select page and start operation */
    MODIFY_REG(
        FLASH->CR, FLASH_CR_PNB, ((page << FLASH_CR_PNB_Pos) | FLASH_CR_PER | FLASH_CR_STRT));

    /* Wait for last operation to be completed */
    furi_check(furi_hal_flash_wait_last_operation(FURI_HAL_FLASH_TIMEOUT));

    /* If operation is completed or interrupted, disable the Page Erase Bit */
    CLEAR_BIT(FLASH->CR, (FLASH_CR_PER | FLASH_CR_PNB));

    /* Flush the caches to be sure of the data consistency */
    furi_hal_flush_cache();

    furi_hal_flash_end(true);

    return true;
}

bool furi_hal_flash_write_dword(size_t address, uint64_t data) {
    furi_hal_flash_begin(false);

    // Ensure that controller state is valid
    furi_check(FLASH->SR == 0);

    /* Check the parameters */
    furi_check(IS_ADDR_ALIGNED_64BITS(address));
    furi_check(IS_FLASH_PROGRAM_ADDRESS(address));

    /* Set PG bit */
    SET_BIT(FLASH->CR, FLASH_CR_PG);

    /* Program first word */
    *(uint32_t*)address = (uint32_t)data;

    // Barrier to ensure programming is performed in 2 steps, in right order
    // (independently of compiler optimization behavior)
    __ISB();

    /* Program second word */
    *(uint32_t*)(address + 4U) = (uint32_t)(data >> 32U);

    /* Wait for last operation to be completed */
    furi_check(furi_hal_flash_wait_last_operation(FURI_HAL_FLASH_TIMEOUT));

    /* If the program operation is completed, disable the PG or FSTPG Bit */
    CLEAR_BIT(FLASH->CR, FLASH_CR_PG);

    furi_hal_flash_end(false);

    return true;
}
