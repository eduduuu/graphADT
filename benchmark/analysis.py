import subprocess

import signal

import time

import csv

import os

import itertools


# 1. Define benchmark parameters

sizes = ['10', '100', '1000', '10000']

duration = '10000'

densities = ['0.3', '0.6', '0.75', '1.0']

representations = ['ADJACENCY_LIST', 'ADJACENCY_MATRIX', 'CSR']

workloads = [ {'name': 'read_only', 'a': '0', 'b': '0', 'c': '100'},

            {'name': 'mostly_read', 'a': '0', 'b': '20', 'c': '80'},

            {'name': 'mostly_write', 'a': '0', 'b': '80', 'c': '20'}]

mode = '0'

csv_filename = "benchmark_cache_llc.csv"


repetitions = 2


def build(representation):

    subprocess.run(['rm', '-rf', 'build/'], cwd='../')


    subprocess.run([

        'cmake', '-B', 'build',

        f"-DIMPLEMENTATION={representation}",

        '-DCMAKE_BUILD_TYPE=Release'

    ], cwd='../')


    subprocess.run(['cmake', '--build', 'build'], cwd='../')


events = ['LLC-loads', 'LLC-load-misses', 'LLC-stores', 'LLC-store-misses', 'page-faults', 'minor-faults', 'major-faults']
#events = ['L1-dcache-loads', 'L1-dcache-load-misses', 'l2_rqsts.references', 'l2_rqsts.miss']


for size, density, workload, representation in itertools.product(

    sizes, densities, workloads, representations

):

    build(representation)

    command = [

        '../build/benchmark/micro_bench',

        '-s', size,

        '-t', duration,

        '-d', density,

        '-m', mode,

        '-a', workload['a'],

        '-b', workload['b'],

        '-c', workload['c'],

        '-g', '0',
        '-i', representation,
        '-k', workload['name'],

        '-w', 'micro_benchmark.csv'

    ]

    print(f"Running: size={size}, duration={duration}, density={density}, mode={mode}, representation={representation}, workload={workload['name']}")

    for i in range(repetitions):

        print(i)

        p = subprocess.Popen(

            command,

            stdin=subprocess.PIPE,

            stdout=subprocess.PIPE,

            stderr=subprocess.STDOUT,

            text=True,

            bufsize=1

        )


        tid = None



        for line in p.stdout:

            if line.startswith("tid"):

                tid = line.strip().split(" ")[1]

                break


        if tid:

            # --- External `perf stat` sync (disabled: micro_bench.c now reads the
            # --- hardware counters itself via perf_event_open). Kept for future use.
            # perf_cmd = [
            #     'perf', 'stat',
            #     '-t', str(tid),
            #     '-x,',
            #     '-e', ','.join(events)
            # ]
            #
            # perf_proc = subprocess.Popen(
            #     perf_cmd,
            #     stdout=subprocess.PIPE,
            #     stderr=subprocess.PIPE,
            #     text=True
            # )
            #
            # time.sleep(0.5)  # let perf attach to the thread before the test starts

            # Tell the C program to start the measured run (it waits on getchar())
            p.stdin.write("\n")
            p.stdin.flush()

            p.wait()

            # perf_proc.send_signal(signal.SIGINT)
            # _, perf_stderr = perf_proc.communicate()
            #
            # perf_data = {}
            # for line in perf_stderr.splitlines():
            #     parts = line.strip().split(',')
            #     if len(parts) >= 3:
            #         value = parts[0].strip()
            #         event_name = parts[2].strip().split(':')[0]
            #         if event_name in events:
            #             perf_data[event_name] = value
            #
            # if not perf_data:
            #     print(f"WARNING: perf captured no events (size={size}, density={density}, "
            #           f"{representation}, {workload['name']}, rep {i}); row skipped")
            #     continue
            #
            # file_exists = os.path.isfile(csv_filename)
            # with open(csv_filename, mode='a', newline='') as f:
            #     writer = csv.writer(f)
            #     if not file_exists:
            #         header = ['size', 'duration', 'density', 'representation', 'workload'] + events
            #         writer.writerow(header)
            #     row = [size, duration, density, representation, workload['name']]
            #     for event in events:
            #         row.append(perf_data.get(event, "0"))
            #     writer.writerow(row)
        else:
            print("WARNING: micro_bench did not report a tid; run skipped")
            p.kill()



