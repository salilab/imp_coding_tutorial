#!/bin/bash -e

# Set up an environment to run CI tests, e.g. with GitHub Actions or Travis

if [ $# -ne 2 ]; then
  echo "Usage: $0 imp_branch python_version"
  exit 1
fi

imp_branch=$1
python_version=$2

# get conda-forge, not main, packages
conda config --remove channels defaults
conda config --add channels conda-forge
if [ ${imp_branch} = "develop" ]; then
  IMP_CONDA="imp-nightly"
else
  IMP_CONDA="imp"
fi
if [ ${python_version} = "2.7" ]; then
  BOOST=""
  pip="pip<=19.3.1"
  # Python.h includes crypt.h, which is no longer provided by default
  crypt="libxcrypt"
else
  BOOST="libboost-devel"
  pip="pip"
  crypt=""
fi
conda create --yes -q -n python${python_version} -c salilab python=${python_version} ${pip} ${crypt} ${IMP_CONDA} ${BOOST} gxx_linux-64 eigen cereal swig cmake numpy
eval "$(conda shell.bash hook)"
conda activate python${python_version}

pip install pytest-cov coverage
