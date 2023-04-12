from balsam.api import ApplicationDefinition, Job
from pathlib import Path, PurePath
from balsam.schemas import JobState
import time
import shutil


class HEPnOS_server(ApplicationDefinition):
    site = "balsam_basics"

    environment_variables = {
        "ICARUSWF_BUILD": "/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build",
        "MPICH_GNI_NDREG_ENTRIES": "1024",
        "MPICH_MAX_THREAD_SAFETY": "multiple",
        "MARGO_ENABLE_PROFILING": "0",
        "MARGO_ENABLE_DIAGNOSTICS": "0",
        "PDOMAIN": "hepnos-sajid",
        "SSGFILE": "hepnos.ssg",
    }

    def shell_preamble(self):
        preamble = str(
            """
        module unload darshan
        module swap PrgEnv-intel PrgEnv-gnu
        module load gcc/9.3.0 
        module swap cray-mpich/7.7.14 cray-mpich-abi/7.7.14
        # activate spack environment
        . /projects/HEP_on_HPC/icaruscode/spack/share/spack/setup-env.sh
        spack env activate icaruscode-09_37_02_vecmt04-hepnos-0_7_2
        # set relevant env vars
        export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
        export FHICL_FILE_PATH=${ICARUSWF_BUILD}/fcl:${FHICL_FILE_PATH}
        export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/lus/theta-fs0/projects/HEP_on_HPC/icaruscode/spack/var/spack/environments/icaruscode-09_37_02_vecmt04-hepnos-0_7_2/.spack-env/view/gdml
        # Setting up protection domain
        apstat -P | grep ${PDOMAIN} || apmgr pdomain -c -u ${PDOMAIN}
        """
        )
        return preamble

    command_template = (
        "-p hepnos-sajid bedrock ofi+gni -c {{ hepnos_config }} -v info &> server-log"
    )

    parameters = {
        "hepnos_config": {"required": True},
    }


HEPnOS_server.sync()

hepnos_config_file: str = (
    "/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build/test/hepnos.json"
)

HEPnOS_server.submit(
    workdir=Path("server"),
    num_nodes=1,
    ranks_per_node=32,
    threads_per_rank=2,
    threads_per_core=1,
    node_packing_count=1,
    hepnos_config=hepnos_config_file,
)


class HEPnOS_list_dbs(ApplicationDefinition):
    site = "balsam_basics"

    environment_variables = {
        "ICARUSWF_BUILD": "/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build",
        "MPICH_GNI_NDREG_ENTRIES": "1024",
        "MPICH_MAX_THREAD_SAFETY": "multiple",
        "MARGO_ENABLE_PROFILING": "0",
        "MARGO_ENABLE_DIAGNOSTICS": "0",
        "PDOMAIN": "hepnos-sajid",
        "SSGFILE": "hepnos.ssg",
    }

    def preprocess(self) -> None:
        ssg_src_file = Path(
            "/projects/HEP_on_HPC/sajid/icarus_hepnos/balsam_basics/data/server/hepnos.ssg"
        )
        while True:
            time.sleep(5)
            print("checking to see if hepnos.ssg exists at source location")
            if ssg_src_file.exists():
                print("hepnos.ssg exists at source location, preprocess done!")
                workdir: PurePath = PurePath(Path.cwd())
                ssg_dst_file = Path(workdir.joinpath("hepnos.ssg"))
                shutil.copyfile(ssg_src_file, ssg_dst_file)

                # Ready to run
                self.job.state = JobState.preprocessed
                break
        return

    def shell_preamble(self):
        preamble = str(
            """
        module unload darshan
        module swap PrgEnv-intel PrgEnv-gnu
        module load gcc/9.3.0 
        module swap cray-mpich/7.7.14 cray-mpich-abi/7.7.14
        # activate spack environment
        . /projects/HEP_on_HPC/icaruscode/spack/share/spack/setup-env.sh
        spack env activate icaruscode-09_37_02_vecmt04-hepnos-0_7_2
        # set relevant env vars
        export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
        export FHICL_FILE_PATH=${ICARUSWF_BUILD}/fcl:${FHICL_FILE_PATH}
        export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/lus/theta-fs0/projects/HEP_on_HPC/icaruscode/spack/var/spack/environments/icaruscode-09_37_02_vecmt04-hepnos-0_7_2/.spack-env/view/gdml
        # Setting up protection domain
        apstat -P | grep ${PDOMAIN} || apmgr pdomain -c -u ${PDOMAIN}
        """
        )
        return preamble

    command_template = (
        "-p hepnos-sajid hepnos-list-databases ofi+gni -s hepnos.ssg > out.json"
    )

    def postprocess(self) -> None:
        workdir: PurePath = PurePath(Path.cwd())
        outfile = Path(workdir.joinpath("out.json"))

        # Wait for creation of connection file
        while True:
            time.sleep(1)
            print("checking to see if connection.json has been created")
            if outfile.exists():
                print("outfile.json has been created")
                break

        # trim last line
        connectionfile = Path(workdir.joinpath("connection.json"))
        with open(connectionfile, "w") as cfile:
            with open(outfile, "r") as ofile:
                lines = ofile.readlines()
            cfile.writelines(lines[:1])

        self.job.state = JobState.postprocessed

        return


HEPnOS_list_dbs.sync()

job_list_dbs = Job.objects.create(
    app_id="HEPnOS_list_dbs",
    site_name="balsam_basics",
    workdir=Path("connection"),
    num_nodes=1,
    ranks_per_node=1,
    threads_per_rank=1,
    threads_per_core=1,
    node_packing_count=1,
)
job_list_dbs.save()

class HEPnOS_queue(ApplicationDefinition):
    site = "balsam_basics"

    environment_variables = {
        "ICARUSWF_BUILD": "/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build",
        "MPICH_GNI_NDREG_ENTRIES": "1024",
        "MPICH_MAX_THREAD_SAFETY": "multiple",
        "MARGO_ENABLE_PROFILING": "0",
        "MARGO_ENABLE_DIAGNOSTICS": "0",
        "PDOMAIN": "hepnos-sajid",
        "SSGFILE": "hepnos.ssg",
    }

    def preprocess(self) -> None:
        connection_src_file = Path(
            "/projects/HEP_on_HPC/sajid/icarus_hepnos/balsam_basics/data/connection/connection.json"
        )

        workdir: PurePath = PurePath(Path.cwd())
        connection_dst_file = Path(workdir.joinpath("connection.json"))
        shutil.copyfile(connection_src_file, connection_dst_file)

        # Ready to run
        self.job.state = JobState.preprocessed

        return

    def shell_preamble(self):
        preamble = str(
            """
        module unload darshan
        module swap PrgEnv-intel PrgEnv-gnu
        module load gcc/9.3.0 
        module swap cray-mpich/7.7.14 cray-mpich-abi/7.7.14
        # activate spack environment
        . /projects/HEP_on_HPC/icaruscode/spack/share/spack/setup-env.sh
        spack env activate icaruscode-09_37_02_vecmt04-hepnos-0_7_2
        # set relevant env vars
        export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
        export FHICL_FILE_PATH=${ICARUSWF_BUILD}/fcl:${FHICL_FILE_PATH}
        export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/lus/theta-fs0/projects/HEP_on_HPC/icaruscode/spack/var/spack/environments/icaruscode-09_37_02_vecmt04-hepnos-0_7_2/.spack-env/view/gdml
        # Setting up protection domain
        apstat -P | grep ${PDOMAIN} || apmgr pdomain -c -u ${PDOMAIN}
        """
        )
        return preamble

    command_template = "-p hepnos-sajid /projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build/src/modules/cheesyQueue_maker ofi+gni connection.json DetSim HitFinding"

HEPnOS_queue.sync()

job_queue = Job.objects.create(
    app_id=HEPnOS_queue.__app_id__,
    site_name="balsam_basics",
    workdir=Path("queue"),
    num_nodes=1,
    ranks_per_node=1,
    threads_per_rank=1,
    threads_per_core=1,
    node_packing_count=1,
    parent_ids=[job_list_dbs.id],
)

class HEPnOS_load(ApplicationDefinition):
    site = "balsam_basics"

    environment_variables = {
        "ICARUSWF_BUILD": "/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build",
        "MPICH_GNI_NDREG_ENTRIES": "1024",
        "MPICH_MAX_THREAD_SAFETY": "multiple",
        "MARGO_ENABLE_PROFILING": "0",
        "MARGO_ENABLE_DIAGNOSTICS": "0",
        "PDOMAIN": "hepnos-sajid",
        "SSGFILE": "hepnos.ssg",
    }

    def preprocess(self) -> None:
        connection_src_file = Path(
            "/projects/HEP_on_HPC/sajid/icarus_hepnos/balsam_basics/data/connection/connection.json"
        )

        workdir: PurePath = PurePath(Path.cwd())
        connection_dst_file = Path(workdir.joinpath("connection.json"))
        shutil.copyfile(connection_src_file, connection_dst_file)

        # Ready to run
        self.job.state = JobState.preprocessed

        return

    def shell_preamble(self):
        preamble = str(
            """
        module unload darshan
        module swap PrgEnv-intel PrgEnv-gnu
        module load gcc/9.3.0 
        module swap cray-mpich/7.7.14 cray-mpich-abi/7.7.14
        # activate spack environment
        . /projects/HEP_on_HPC/icaruscode/spack/share/spack/setup-env.sh
        spack env activate icaruscode-09_37_02_vecmt04-hepnos-0_7_2
        # set relevant env vars
        export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
        export FHICL_FILE_PATH=${ICARUSWF_BUILD}/fcl:${FHICL_FILE_PATH}
        export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/lus/theta-fs0/projects/HEP_on_HPC/icaruscode/spack/var/spack/environments/icaruscode-09_37_02_vecmt04-hepnos-0_7_2/.spack-env/view/gdml
        # Setting up protection domain
        apstat -P | grep ${PDOMAIN} || apmgr pdomain -c -u ${PDOMAIN}
        """
        )
        return preamble

    command_template = "-p hepnos-sajid /projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build/src/modules/mpi_wrapper -H --num_evts_per_rank 1 --root_file_path {{ input_filename }}"

    parameters = {
        "input_filename": {"required": True},
    }


HEPnOS_load.sync()

data_dir = "/projects/HEP_on_HPC/sajid/icarus_hepnos/data/"
for i in range(3):
    input_file = data_dir + f"""detsim_{i:02d}.root"""
    job_load = Job.objects.create(
        app_id=HEPnOS_load.__app_id__,
        site_name="balsam_basics",
        workdir=Path(f"""detsim_{i:02d}"""),
        num_nodes=1,
        ranks_per_node=32,
        threads_per_rank=2,
        threads_per_core=1,
        node_packing_count=1,
        parent_ids=[job_queue.id],
        parameters={"input_filename": input_file},
    )


class HEPnOS_process(ApplicationDefinition):
    site = "balsam_basics"

    environment_variables = {
        "ICARUSWF_BUILD": "/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build",
        "MPICH_GNI_NDREG_ENTRIES": "1024",
        "MPICH_MAX_THREAD_SAFETY": "multiple",
        "MARGO_ENABLE_PROFILING": "0",
        "MARGO_ENABLE_DIAGNOSTICS": "0",
        "PDOMAIN": "hepnos-sajid",
        "SSGFILE": "hepnos.ssg",
    }

    def preprocess(self) -> None:
        connection_src_file = Path(
            "/projects/HEP_on_HPC/sajid/icarus_hepnos/balsam_basics/data/connection/connection.json"
        )

        workdir: PurePath = PurePath(Path.cwd())
        connection_dst_file = Path(workdir.joinpath("connection.json"))
        shutil.copyfile(connection_src_file, connection_dst_file)

        # Ready to run
        self.job.state = JobState.preprocessed

        return

    def shell_preamble(self):
        preamble = str(
            """
        module unload darshan
        module swap PrgEnv-intel PrgEnv-gnu
        module load gcc/9.3.0 
        module swap cray-mpich/7.7.14 cray-mpich-abi/7.7.14
        # activate spack environment
        . /projects/HEP_on_HPC/icaruscode/spack/share/spack/setup-env.sh
        spack env activate icaruscode-09_37_02_vecmt04-hepnos-0_7_2
        # set relevant env vars
        export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
        export FHICL_FILE_PATH=${ICARUSWF_BUILD}/fcl:${FHICL_FILE_PATH}
        export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/lus/theta-fs0/projects/HEP_on_HPC/icaruscode/spack/var/spack/environments/icaruscode-09_37_02_vecmt04-hepnos-0_7_2/.spack-env/view/gdml
        # Setting up protection domain
        apstat -P | grep ${PDOMAIN} || apmgr pdomain -c -u ${PDOMAIN}
        """
        )
        return preamble

    command_template = "-p hepnos-sajid /projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build/src/modules/mpi_wrapper -H -p -t {{client_hwthreads_perrank}} --num_evts_per_rank 1"

    parameters = {
        "client_hwthreads_perrank": {"required": True},
    }


HEPnOS_process.sync()

job_process = Job.objects.create(
    app_id=HEPnOS_process.__app_id__,
    site_name="balsam_basics",
    workdir=Path("process"),
    num_nodes=3,
    ranks_per_node=32,
    threads_per_rank=2,
    threads_per_core=1,
    node_packing_count=1,
    parent_ids=[job_list_dbs.id],
    parameters={"client_hwthreads_perrank": 2},
)
