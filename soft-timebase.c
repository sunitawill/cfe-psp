/**
 * \file
 *
 * A PSP module that instantiates an RTOS-backed OSAL timebase to provide
 * various timing services.  This in turn may be used to drive CFE TIME 1Hz
 * signal, the CFS SCH major/minor frame sync and other periodic services
 * in CFE.
 *
 * This module can be used on systems which do not have a hardware
 * source for the 1Hz signal or timing info (i.e. simulation, test
 * and debug platforms, etc).
 */

#include "cfe_psp.h"
#include "cfe_psp_module.h"
#include "cfe_psp_config.h"

CFE_PSP_MODULE_DECLARE_SIMPLE(soft_timebase);

/*
 * Global state data for this module (not exposed publicly)
 */
static struct
{
    osal_id_t sys_timebase_id;
} PSP_SoftTimebase_Global;

void soft_timebase_Init(uint32 PspModuleId)
{
    int32 status;

    memset(&PSP_SoftTimebase_Global, 0, sizeof(PSP_SoftTimebase_Global));

    /* Set up the OSAL timebase using the well-known name */
    status = OS_TimeBaseCreate(&PSP_SoftTimebase_Global.sys_timebase_id, CFE_PSP_SOFT_TIMEBASE_NAME, NULL);
    if (status == OS_SUCCESS)
    {
        /* Set the timebase to trigger with desired resolution */
        status = OS_TimeBaseSet(PSP_SoftTimebase_Global.sys_timebase_id, CFE_PSP_SOFT_TIMEBASE_PERIOD,
                                CFE_PSP_SOFT_TIMEBASE_PERIOD);
    }

    /*
     * The only way this can fail is through a misconfiguration or API incompatibility -
     * if it fails, it means all timing related functions are likely to be broken,
     * CFE TIME may not work correctly, and background jobs will not run.
     *
     * Might even be worth a CFE_PSP_Panic(), but it still may be possible
     * to boot CFE and (maybe) save the system by uploading a file with the bug fixed.
     */
    if (status != OS_SUCCESS)
    {
        printf("CFE_PSP: *** Failed to configure software timebase \'%s\', status = %d! ***\n",
               CFE_PSP_SOFT_TIMEBASE_NAME, (int)status);
    }
    else
    {
        /* Inform the user that this module is in use */
        printf("CFE_PSP: Instantiated software timebase \'%s\' running at %lu usec\n", CFE_PSP_SOFT_TIMEBASE_NAME,
               (unsigned long)CFE_PSP_SOFT_TIMEBASE_PERIOD);
    }
}
