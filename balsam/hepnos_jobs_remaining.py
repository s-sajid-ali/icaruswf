from balsam.api import Job
from pathlib import Path
import site_def

PDOMAIN = "hepnos-sajid"

hepnos_config_file: str = (
    "/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build/test/hepnos.json"
)

data_dir = str("/projects/HEP_on_HPC/sajid/icarus_hepnos/data_split_0/")

# Generate a dict of all filenames and number of events in them!
commands_file = open("/projects/HEP_on_HPC/commands.txt", "r")
lines = commands_file.readlines()
file_dict = {}
for line in lines:
    chunks = line.split()
    events = int(chunks[4])
    filename_full = chunks[-3]
    filename = filename_full.split("/")[-1]
    file_dict[filename] = events


job_list_dbs = Job.objects.create(
    app_id="HEPnOS_list_dbs",
    site_name=site_def.SITE_NAME,
    workdir=Path("connection" + "_" + str(site_def.PDOMAIN)),
    num_nodes=1,
    ranks_per_node=1,
    threads_per_rank=1,
    threads_per_core=1,
    node_packing_count=1,
    parameters={"pdomain": site_def.PDOMAIN},
)
assert job_list_dbs.id is not None

job_queue = Job.objects.create(
    app_id="HEPnOS_queue",
    site_name=site_def.SITE_NAME,
    workdir=Path("queue" + "_" + str(site_def.PDOMAIN)),
    num_nodes=1,
    ranks_per_node=1,
    threads_per_rank=1,
    threads_per_core=1,
    node_packing_count=1,
    parent_ids=[job_list_dbs.id],
    parameters={"pdomain": site_def.PDOMAIN},
)
assert job_queue.id is not None


job_load = [
    Job(
        app_id="HEPnOS_load",
        site_name=site_def.SITE_NAME,
        workdir=Path(item[0] + "_" + str(site_def.PDOMAIN)),
        num_nodes=1,
        ranks_per_node=item[1],
        threads_per_rank=2,
        threads_per_core=1,
        node_packing_count=1,
        parent_ids=[job_queue.id],
        parameters={
            "pdomain": site_def.PDOMAIN,
            "input_filename": data_dir + item[0],
        },
    )
    for item in file_dict.items()
]
job_load = Job.objects.bulk_create(job_load)
for job in job_load:
    assert job.id is not None

job_process = Job.objects.create(
    app_id="HEPnOS_process",
    site_name=site_def.SITE_NAME,
    workdir=Path("process" + "_" + str(site_def.PDOMAIN)),
    num_nodes=6,
    ranks_per_node=32,
    threads_per_rank=2,
    threads_per_core=1,
    node_packing_count=1,
    parent_ids=[job.id for job in job_load],
    parameters={"pdomain": site_def.PDOMAIN, "client_hwthreads_perrank": 2},
)
assert job_process.id is not None


job_shutdown = Job.objects.create(
    app_id="HEPnOS_shutdown",
    site_name=site_def.SITE_NAME,
    workdir=Path("shutdown" + "_" + str(site_def.PDOMAIN)),
    num_nodes=1,
    ranks_per_node=1,
    threads_per_rank=1,
    threads_per_core=1,
    node_packing_count=1,
    parent_ids=[job_process.id],
    parameters={"pdomain": site_def.PDOMAIN},
)
assert job_shutdown.id is not None
