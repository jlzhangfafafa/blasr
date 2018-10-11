## Running BLASR

  Typing 'blasr -h' or 'blasr -help' on the command line will give you a
  list of options.  At the least, provide reads and a genome, where reads file format can be
  bam|dataset|fasta|fastq|bas.h5, and genome file format can be fasta|ReferenceSet.

### Some typical use cases

Align subreads in movie.subreads.bam to ecoli_K12 genome, and output in BAM format.

    blasr movie.subreads.bam ecoli_K12.fasta --bam --out alignments.bam

Align subreads in movie.subreadset.xml to ecoli_K12 genome, and output in BAM format.

    blasr movie.subreadset.xml ecoli_K12.fasta --bam --out alignments.bam

Align subreads in movie.subreadset.xml to ecoli_K12 genome ReferenceSet, and output in BAM format.

    blasr movie.subreadset.xml ecoli_K12.referenceset.xml --bam --out alignments.bam

Align CCS reads in movie.consensusreadset.xml to ecoli_K12 genome, and output in BAM format.

    blasr movie.consensusreadset.xml ecoli_K12.fasta --bam --out alignments.bam

Use multiple threads, e.x., 16 threads

    blasr movie.subreads.bam ecoli_K12.fasta --nproc 16

Include a larger minimal match, for faster but less sensitive alignments

    blasr movie.subreads.bam ecoli_K12.fasta --minMatch 15

Produce alignments in a pairwise human readable format

    blasr movie.subreads.bam ecoli_K12.fasta -m 0

Use a precomputed suffix array for faster startup

    sawriter ecoli_K12.fasta.sa ecoli_K12.fasta #First precompute the suffix array
    blasr movie.subreads.bam ecoli_K12.fasta --sa ecoli_K12.fasta.sa

Align RSII reads from reads.bas.h5 to ecoli_K12 genome, and output in SAM format.

    blasr reads.bas.h5  ecoli_K12.fasta --sam --out alignments.sam

Same as above, but with soft clipping

    blasr reads.bas.h5  ecoli_K12.fasta --sam --clipping soft --out alignments.sam
