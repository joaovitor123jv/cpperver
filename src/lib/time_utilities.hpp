#ifndef __CERVER_TIME_UTILITIES
#define __CERVER_TIME_UTILITIES

#include <iostream>
#include <stdio.h>
#include <time.h>

class Stopwatch {
    private:
        clock_t started_at;
        clock_t finished_at;
        clock_t created_at;
    public:
        Stopwatch() {
            this->reset_times();
        }

        Stopwatch(bool track_creation_date) {
            if(track_creation_date) {
                this->finished_at = 0;
                this->started_at = 0;
                this->created_at = clock();
            } else reset_times();
        }

        void reset_times() {
            this->finished_at = 0;
            this->started_at = 0;
            this->created_at = 0;
        }

        void start() {
            this->started_at = clock();
        }

        void finish() {
            this->finished_at = clock();
        }

        void print_elapsed_time() {
            if(this->finished_at != 0 && this->finished_at != 0)
            {
                double elapsed_time = (double)(this->finished_at - this->started_at)/CLOCKS_PER_SEC;

                if(elapsed_time < 1.0 && elapsed_time >= 0.001) {
                    fprintf(
                        stdout,
                        "Elapsed time (start -> finish): %.5fms\n",
                        elapsed_time*1000.0
                    );
                } else if(elapsed_time < 0.001) {
                    fprintf(
                        stdout,
                        "Elapsed time (start -> finish): %.5fns\n",
                        elapsed_time*1000.0*1000.0
                    );
                } else {
                    fprintf(
                        stdout,
                        "Elapsed time (start -> finish): %.5fs\n",
                        elapsed_time
                    );
                }
                
            }
            else if (this->finished_at == 0 && (this->started_at != 0 && this->created_at != 0))
            {
                double elapsed_time = (double)(this->finished_at - this->started_at)/CLOCKS_PER_SEC;

                if(elapsed_time < 1.0 && elapsed_time >= 0.01) {
                    fprintf(
                        stdout,
                        "Elapsed time (created -> started): %.5fms\n",
                        elapsed_time*1000.0
                    );
                } else if(elapsed_time < 0.001) {
                    fprintf(
                        stdout,
                        "Elapsed time (created -> started): %.5fns\n",
                        elapsed_time*1000.0*1000.0
                    );
                } else {
                    fprintf(
                        stdout,
                        "Elapsed time (created -> started): %.5fs\n",
                        elapsed_time
                    );
                }
            }
            
            fprintf(stdout, "Timer not started or insufficient info\n");
        }
};

#endif // __CERVER_TIME_UTILITIES