#include "hotcall.h"
#include "enclave-utils.h"

int
ecall_start_poller(async_ecall * ctx){
    // sgx_thread_mutex_init(&ctx->mutex, NULL);
    char buf[128];

    while (1) {
        sgx_spin_lock(&ctx->spinlock);

        if (ctx->run) {
            #ifdef TIMEOUT
            timeout_counter = INIT_TIMER_VALUE;
            #endif
            ENCLAVE_LOG(buf, "Running function %d\n", ctx->function);
            ctx->run = false;
            execute_function(ctx->function, ctx->args, ctx->ret);
            ctx->is_done = true;
            sgx_spin_unlock(&ctx->spinlock);
            ENCLAVE_LOG(buf, "Running function %d done.\n", ctx->function);
            continue;
        }

        sgx_spin_unlock(&ctx->spinlock);

        // Its recommended by intel to add pause actions inside spinlock loops in order to increase performance.
        for (int i = 0; i < 3; ++i) {
            __asm
            __volatile(
              "pause"
            );
        }

        #ifdef TIMEOUT
        timeout_counter--;
        if (timeout_counter <= 0) {
            printf("HOTCALL SERVICE SLEEPING.\n");
            timeout_counter = INIT_TIMER_VALUE;
            ctx->sleeping   = true;
            ocall_sleep();
            ctx->sleeping = false;
            // sgx_thread_mutex_lock(&ctx->mutex);
            // sgx_thread_cond_wait(&ctx->cond, &ctx->mutex
            // sgx_thread_mutex_unlock(&ctx->mutex);
        }
        #endif /* ifdef TIMEOUT */
    }


    sgx_spin_unlock(&ctx->spinlock);

    return 0;
} /* ecall_start_poller */