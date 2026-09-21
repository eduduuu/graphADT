import subprocess
import signal
import time
import csv
import os
import itertools

# 1. Define benchmark parameters
sizes = ['1000', '10000', '100000']
duration = '10000'
densities = ['0.3', '0.6', '0.75', '1.0']
representations = ['ADJACENCY_LIST', 'ADJACENCY_MATRIX', 'CSR']
workloads = [ {'name': 'read_only', 'a': '0', 'b': '0', 'c': '100'},
            {'name': 'mostly_read', 'a': '5', 'b': '15', 'c': '80'},
            {'name': 'mostly_write', 'a': '20', 'b': '60', 'c': '20'}]
mode = '0'
csv_filename = "benchmark_cache.csv"

repetitions = 2

def build(representation):
    subprocess.run(['rm', '-rf', 'build/'], cwd='../')

    subprocess.run([
        'cmake', '-B', 'build',
        f"-DIMPLEMENTATION={representation}",
        '-DCMAKE_BUILD_TYPE=Debug'
    ], cwd='../')

    subprocess.run(['cmake', '--build', 'build'], cwd='../')

events = ['LLC-loads', 'LLC-load-misses', 'LLC-stores', 'LLC-store-misses', 'page-faults', 'minor-faults', 'major-faults']

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
        '-w', f"{workload['name']}_{representation}_{size}_{density}.csv"
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
            perf_cmd = [
                'perf', 'stat',
                '-t', str(tid),
                '-x,',
                '-e', ','.join(events)
            ]

            perf_proc = subprocess.Popen(
                perf_cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )

            time.sleep(0.05)

            p.stdin.write("\n")
            p.stdin.flush()

            p.wait()

            perf_proc.send_signal(signal.SIGINT)
            _, perf_stderr = perf_proc.communicate()

            perf_data = {}
            for line in perf_stderr.splitlines():
                parts = line.strip().split(',')
                if len(parts) >= 3:
                    value = parts[0].strip()
                    event_name = parts[2].strip().split(':')[0]
                    if event_name in events:
                        perf_data[event_name] = value

            file_exists = os.path.isfile(csv_filename)

            with open(csv_filename, mode='a', newline='') as f:
                writer = csv.writer(f)

                if not file_exists:
                    header = ['size', 'duration', 'density', 'representation', 'workload'] + events
                    writer.writerow(header)

                row = [size, duration, density, representation, workload['name']]
                for event in events:
                    row.append(perf_data.get(event, "0"))

                writer.writerow(row)

