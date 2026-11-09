#!/bin/bash -e

# Set up an environment to run CI tests, e.g. with GitHub Actions or Travis

if [ $# -ne 1 ]; then
  echo "Usage: $0 imp_branch"
  exit 1
fi

imp_branch=$1

if [ ${imp_branch} = "develop" ]; then
  IMP_CONDA="imp-nightly"
  JAX="jax"
else
  IMP_CONDA="imp"
  JAX=""
fi
conda activate test
conda install --yes -q pip ${IMP_CONDA} libboost-devel gxx_linux-64 eigen cereal swig cmake numpy ${JAX}

pip install pytest-cov coverage
