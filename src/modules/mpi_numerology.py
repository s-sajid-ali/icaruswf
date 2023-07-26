from __future__ import print_function


def calculate_slice_for_rank(myrank, nranks, arraysz):
    """Calculate the slice indices for array processing in MPI programs.

    Return (low, high), a tuple containing the range of indices
    in an array of size arraysz, to be processed by MPI rank myrank of
    a total nranks. We assure as equitable a distribution of ranks as
    possible.
    """

    if myrank >= nranks:
        raise Exception("myrank must be less than nranks")

    # Each rank will get either minsize or minsize+1 elements to work on.
    minsize, leftovers = divmod(arraysz, nranks)

    # Ranks [0, leftovers) get minsize+1 elements
    # Ranks [leftovers, nranks) get minsize elements
    slice_size = minsize + 1 if myrank < leftovers else minsize

    if myrank < leftovers:
        low = myrank * slice_size
        high = low + slice_size
    else:
        # The calculation of 'low' is the algebraically simplified version of
        # the more obvious:
        #  low = leftovers*(my_size_size_bytes + 1) + (myrank - leftovers)*my_size_size_bytes
        low = leftovers + myrank * slice_size
        high = low + slice_size
    return (low, high)

def ranks_by_slot(nranks, arraysz):
    """return a list of length nranks, telling which array entry will be processed by each rank
    """
    # Each file will be processed by either min_ranks_per_entry or min_ranks_per_entry+1 
    mre, leftovers = divmod(nranks, arraysz)
    # first figure out how many ranks will work on each slot.
    # slots [0, leftovers) get mre+1 ranks
    # slots [leftovers, arraysz) get mre ranks
    slots = [ mre+1 if i < leftovers else mre for i in range(arraysz)]
    res = []
    for i, s in enumerate(slots):
        res.append([i]*s)
    return [num for elem in res for num in elem]

def calculate_rank_for_array_entry(myrank, nranks, arraysz):
    """Calculate which one array entry this rank should be working on
    """
    assert arraysz <= nranks
    assert myrank < nranks

    return ranks_by_slot(nranks, arraysz)[myrank]

