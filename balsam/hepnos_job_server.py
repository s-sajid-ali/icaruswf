from balsam.api import Job
from pathlib import Path
import site_def

hepnos_config_file: str = (
    "/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build/test/hepnos.json"
)

job_server = Job.objects.create(
    app_id="HEPnOS_server",
    site_name=site_def.SITE_NAME,
    workdir=Path("server" + "_" + str(site_def.PDOMAIN)),
    num_nodes=128,
    ranks_per_node=32,
    threads_per_rank=2,
    threads_per_core=1,
    node_packing_count=1,
    parameters={"pdomain": site_def.PDOMAIN, "hepnos_config": hepnos_config_file},
)

assert job_server.id is not None
