#!/bin/bash -e

# Set up an environment to run tests under Travis CI (see ../.travis.yml)

if [ $# -ne 2 ]; then
  echo "Usage: $0 imp_branch python_version"
  exit 1
fi

cur_dir=$(pwd)
imp_branch=$1
python_version=$2
temp_dir=$(mktemp -d)

if [ ${imp_branch} = "develop" ]; then
  IMP_CONDA="imp-nightly"
  # get conda-forge, not main, packages
  conda config --remove channels defaults
  conda config --add channels conda-forge
else
  IMP_CONDA="imp"
fi

cd ${temp_dir}

conda create --yes -q -n python${python_version} -c salilab python=${python_version} pip nose ${IMP_CONDA} gxx_linux-64 eigen swig cmake
eval "$(conda shell.bash hook)"
conda activate python${python_version}
pip install coverage

# IMP tests use sys.argv[0] to determine their location, which won't work if
# we use nosetests, so add a workaround
ln -sf $(which nosetests) ${cur_dir}/modules/foo/test/

cd ${cur_dir}

rm -rf ${temp_dir}
