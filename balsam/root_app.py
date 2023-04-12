from balsam.api import ApplicationDefinition
from pathlib import Path


class ROOT(ApplicationDefinition):
    site = "balsam_basics"

    environment_variables = {
        "ICARUSWF_BUILD": "/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build",
        "MPICH_GNI_NDREG_ENTRIES": "1024",
        "MPICH_MAX_THREAD_SAFETY": "multiple",
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
        spack env activate icaruscode-09_37_02_vecmt04-hepnos-0_7_1
        # set relevant env vars
        export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
        export FHICL_FILE_PATH=${ICARUSWF_BUILD}/fcl:${FHICL_FILE_PATH}
        export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/lus/theta-fs0/projects/HEP_on_HPC/icaruscode/spack/var/spack/environments/icaruscode-09_37_02_vecmt04-hepnos-0_7_1/.spack-env/view/gdml
        """
        )
        return preamble

    command_template = "/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build/src/modules/mpi_wrapper -R -t 2 --num_evts_per_rank 1 --root_file_path {{ input_filename }}"

    parameters = {
        "input_filename": {"required": True},
    }


ROOT.sync()

"""
input_file = "/projects/HEP_on_HPC/icarus_data/icaruscode-v09_37_01_02p02/icaruscode-09_37_01_02p02-samples/prodcorsika_bnb_genie_protononly_overburden_icarus_20220118T213827-GenBNBbkgr_100evt_G4_DetSim.root" 
ROOT.submit(workdir=Path("100evt"),
            num_nodes=1,
            ranks_per_node=32,
            threads_per_rank=2,
            threads_per_core=1,
            node_packing_count=1,
            input_filename=input_file)
"""

data_dir = "/projects/HEP_on_HPC/sajid/icarus_hepnos/data/"
for i in range(4):
    input_file = data_dir + f"""detsim_{i:02d}.root"""
    ROOT.submit(
        workdir=Path(f"""detsim_{i:02d}"""),
        num_nodes=1,
        ranks_per_node=32,
        threads_per_rank=2,
        threads_per_core=1,
        node_packing_count=1,
        input_filename=input_file,
    )
