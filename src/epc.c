#define TRACE_MODULE _epc_main

#include "core_general.h"
#include "core_debug.h"
#include "core_semaphore.h"

#include "context.h"
#include "app.h"

static semaphore_id pcrf_sem1 = 0;
static semaphore_id pcrf_sem2 = 0;

static semaphore_id pgw_sem1 = 0;
static semaphore_id pgw_sem2 = 0;

static semaphore_id sgw_sem1 = 0;
static semaphore_id sgw_sem2 = 0;

static semaphore_id hss_sem1 = 0;
static semaphore_id hss_sem2 = 0;

const char *app_name = "epc";

status_t app_initialize(const char *config_path, const char *log_path)
{
    pid_t pid;
    status_t rv;
    int others = 0;

    rv = app_will_initialize(config_path, log_path);
    if (rv != CORE_OK) return rv;

    others = context_self()->trace_level.others;
    if (others)
    {
        d_trace_level(&_epc_main, others);
    }


    /************************* PCRF Process **********************/
    semaphore_create(&pcrf_sem1, 0); /* copied to PCRF/PGW/SGW/HSS process */
    semaphore_create(&pcrf_sem2, 0); /* copied to PCRF/PGW/SGW/HSS process */

    if (context_self()->node.disable_pcrf == 0)
    {
        pid = fork();
        d_assert(pid >= 0, _exit(EXIT_FAILURE), "fork() failed");

        if (pid == 0)
        {
            d_trace(1, "PCRF try to initialize\n");
            rv = pcrf_initialize();
            d_assert(rv == CORE_OK,, "Failed to intialize PCRF");
            d_trace(1, "PCRF initialize...done\n");

            if (pcrf_sem1) semaphore_post(pcrf_sem1);
            if (pcrf_sem2) semaphore_wait(pcrf_sem2);

            if (rv == CORE_OK)
            {
                d_trace(1, "PCRF try to terminate\n");
                pcrf_terminate();
                d_trace(1, "PCRF terminate...done\n");
            }

            if (pcrf_sem1) semaphore_post(pcrf_sem1);

            /* allocated from parent process */
            if (pcrf_sem1) semaphore_delete(pcrf_sem1);
            if (pcrf_sem2) semaphore_delete(pcrf_sem2);

            app_did_terminate();

            core_terminate();

            _exit(EXIT_SUCCESS);
        }

        if (pcrf_sem1) semaphore_wait(pcrf_sem1);
    }


    /************************* PGW Process **********************/

    semaphore_create(&pgw_sem1, 0); /* copied to PGW/SGW/HSS process */
    semaphore_create(&pgw_sem2, 0); /* copied to PGW/SGW/HSS process */

    if (context_self()->node.disable_pgw == 0)
    {
        pid = fork();
        d_assert(pid >= 0, _exit(EXIT_FAILURE), "fork() failed");

        if (pid == 0)
        {
            /* allocated from parent process */
            if (pcrf_sem1) semaphore_delete(pcrf_sem1);
            if (pcrf_sem2) semaphore_delete(pcrf_sem2);

            d_trace(1, "PGW try to initialize\n");
            rv = pgw_initialize();
            d_assert(rv == CORE_OK,, "Failed to intialize PGW");
            d_trace(1, "PGW initialize...done\n");

            if (pgw_sem1) semaphore_post(pgw_sem1);
            if (pgw_sem2) semaphore_wait(pgw_sem2);

            if (rv == CORE_OK)
            {
                d_trace(1, "PGW try to terminate\n");
                pgw_terminate();
                d_trace(1, "PGW terminate...done\n");
            }

            if (pgw_sem1) semaphore_post(pgw_sem1);

            /* allocated from parent process */
            if (pgw_sem1) semaphore_delete(pgw_sem1);
            if (pgw_sem2) semaphore_delete(pgw_sem2);

            app_did_terminate();

            core_terminate();

            _exit(EXIT_SUCCESS);
        }

        if (pgw_sem1) semaphore_wait(pgw_sem1);
    }


    /************************* SGW Process **********************/

    semaphore_create(&sgw_sem1, 0); /* copied to SGW/HSS process */
    semaphore_create(&sgw_sem2, 0); /* copied to SGW/HSS process */

    if (context_self()->node.disable_sgw == 0)
    {
        pid = fork();
        d_assert(pid >= 0, _exit(EXIT_FAILURE), "fork() failed");

        if (pid == 0)
        {
            /* allocated from parent process */
            if (pcrf_sem1) semaphore_delete(pcrf_sem1);
            if (pcrf_sem2) semaphore_delete(pcrf_sem2);
            if (pgw_sem1) semaphore_delete(pgw_sem1);
            if (pgw_sem2) semaphore_delete(pgw_sem2);

            d_trace(1, "SGW try to initialize\n");
            rv = sgw_initialize();
            d_assert(rv == CORE_OK,, "Failed to intialize SGW");
            d_trace(1, "SGW initialize...done\n");

            if (sgw_sem1) semaphore_post(sgw_sem1);
            if (sgw_sem2) semaphore_wait(sgw_sem2);

            if (rv == CORE_OK)
            {
                d_trace(1, "SGW try to terminate\n");
                sgw_terminate();
                d_trace(1, "SGW terminate...done\n");
            }

            if (sgw_sem1) semaphore_post(sgw_sem1);

            /* allocated from parent process */
            if (sgw_sem1) semaphore_delete(sgw_sem1);
            if (sgw_sem2) semaphore_delete(sgw_sem2);

            app_did_terminate();

            core_terminate();

            _exit(EXIT_SUCCESS);
        }

        if (sgw_sem1) semaphore_wait(sgw_sem1);
    }


    /************************* HSS Process **********************/

    semaphore_create(&hss_sem1, 0); /* copied to HSS process */
    semaphore_create(&hss_sem2, 0); /* copied to HSS process */

    if (context_self()->node.disable_hss == 0)
    {
        pid = fork();
        d_assert(pid >= 0, _exit(EXIT_FAILURE), "fork() failed");

        if (pid == 0)
        {
            /* allocated from parent process */
            if (pcrf_sem1) semaphore_delete(pcrf_sem1);
            if (pcrf_sem2) semaphore_delete(pcrf_sem2);
            if (pgw_sem1) semaphore_delete(pgw_sem1);
            if (pgw_sem2) semaphore_delete(pgw_sem2);
            if (sgw_sem1) semaphore_delete(sgw_sem1);
            if (sgw_sem2) semaphore_delete(sgw_sem2);

            d_trace(1, "HSS try to initialize\n");
            rv = hss_initialize();
            d_assert(rv == CORE_OK,, "Failed to intialize HSS");
            d_trace(1, "HSS initialize...done\n");

            if (hss_sem1) semaphore_post(hss_sem1);
            if (hss_sem2) semaphore_wait(hss_sem2);

            if (rv == CORE_OK)
            {
                d_trace(1, "HSS try to terminate\n");
                hss_terminate();
                d_trace(1, "HSS terminate...done\n");
            }

            if (hss_sem1) semaphore_post(hss_sem1);

            if (hss_sem1) semaphore_delete(hss_sem1);
            if (hss_sem2) semaphore_delete(hss_sem2);

            app_did_terminate();

            core_terminate();

            _exit(EXIT_SUCCESS);
        }

        if (hss_sem1) semaphore_wait(hss_sem1);
    }

    rv = app_did_initialize();
    if (rv != CORE_OK) return rv;

    d_trace(1, "MME try to initialize\n");
    rv = mme_initialize();
    d_assert(rv == CORE_OK, return rv, "Failed to intialize MME");
    d_trace(1, "MME initialize...done\n");

    return CORE_OK;;
}

void app_terminate(void)
{
    app_will_terminate();

    d_trace(1, "MME try to terminate\n");
    mme_terminate();
    d_trace(1, "MME terminate...done\n");

    if (context_self()->node.disable_hss == 0)
    {
        if (hss_sem2) semaphore_post(hss_sem2);
        if (hss_sem1) semaphore_wait(hss_sem1);
    }
    if (hss_sem1) semaphore_delete(hss_sem1);
    if (hss_sem2) semaphore_delete(hss_sem2);

    if (context_self()->node.disable_sgw == 0)
    {
        if (sgw_sem2) semaphore_post(sgw_sem2);
        if (sgw_sem1) semaphore_wait(sgw_sem1);
    }
    if (sgw_sem1) semaphore_delete(sgw_sem1);
    if (sgw_sem2) semaphore_delete(sgw_sem2);

    if (context_self()->node.disable_pgw == 0)
    {
        if (pgw_sem2) semaphore_post(pgw_sem2);
        if (pgw_sem1) semaphore_wait(pgw_sem1);
    }
    if (pgw_sem1) semaphore_delete(pgw_sem1);
    if (pgw_sem2) semaphore_delete(pgw_sem2);

    if (context_self()->node.disable_pcrf == 0)
    {
        if (pcrf_sem2) semaphore_post(pcrf_sem2);
        if (pcrf_sem1) semaphore_wait(pcrf_sem1);
    }
    if (pcrf_sem1) semaphore_delete(pcrf_sem1);
    if (pcrf_sem2) semaphore_delete(pcrf_sem2);

    app_did_terminate();
}
