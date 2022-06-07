# Environment and Setup
## [Initial Chipyard Setup](https://chipyard.readthedocs.io/en/latest/Chipyard-Basics/Initial-Repo-Setup.html#setting-up-the-chipyard-repo)
Running the following commands will clone and setup the chipyard repo locally.
```
git clone https://github.com/ucb-bar/chipyard.git
cd chipyard
git checkout 481398b910fa95ec88dd578c67ba358a4d83129d # Stable at time of writing
./scripts/init-submodules-no-riscv-tools.sh
```
Add this extra step to set variable for main chipyard directory for later use.
```
export CHIPYARD_MAIN=$(pwd)
```
## [Toolchain](https://chipyard.readthedocs.io/en/latest/Chipyard-Basics/Initial-Repo-Setup.html#building-a-toolchain)
Running the following script will the Hwacha toolchain.
```
./scripts/build-toolchains.sh esp-tools
```
After the toolchain is built, set environment variables using `env.sh` by running:
```
source ./env.sh
```
OR
```
echo "source ./env.sh" >> ~/.bashrc
source ~/.bashrc
```
to set the variables permanently.

## [Main HD Vectorization Code](https://github.com/alisha-menon/cs252_ee290_project)
Running the following commands will clone the repo with the main HD Vectorization code and copy over the necessary files into the chipyard directory. <br>
*Make sure you clone this repo outside the main chiyard directory*
```
git clone https://github.com/alisha-menon/cs252_ee290_project.git HD_Vec
cd HD_Vec
cp -R generators/HD_sensor_fusion/ $CHIPYARD_MAIN/generators/
cp -R Verilog\ HD\ Sensor\ Fusion/ $CHIPYARD_MAIN/
cp -R MATLAB\ HD\ Sensor\ Fusion/ $CHIPYARD_MAIN/
```
### More environment Variables (VERY IMPORTANT)
Setting these environment variables will make work much easier.

```
export VCS_SRC=$CHIPYARD_MAIN/sims/vcs
export CODE_SRC=$CHIPYARD_MAIN/generators/HD_sensor_fusion/software/fusion
```
OR
```
echo "export VCS_SRC=$CHIPYARD_MAIN/sims/vcs" >> ~/.bashrc
echo "export CODE_SRC=$CHIPYARD_MAIN/generators/HD_sensor_fusion/software/fusion" >> ~/.bashrc
source ~/.bashrc
```
to set the variables permanently.

## Configure Dependency Submodules
```
cd $CHIPYARD_MAIN/generators
rm -r esp-tests/
git clone https://github.com/ucb-bar/esp-tests.git
cd esp-tests/
rm -r env
git clone https://github.com/ucb-bar/esp-test-env env
```

## Compile Main Code
```
riscv64-unknown-elf-gcc $CODE_SRC/main_vec.c $CODE_SRC/fusion_funcs.c $CODE_SRC/fusion_funcs32.c $CODE_SRC/computeNgram_og.c $CODE_SRC/computeNgram_og32.c $CODE_SRC/associative_memory_vec.c $CODE_SRC/associative_memory.c $CODE_SRC/associative_memory32.c $CODE_SRC/vec_computeNgram.c $CODE_SRC/vec_hamming_distance.c $CODE_SRC/ngram_bind_asm.S $CODE_SRC/vec_xor_asm.S $CODE_SRC/vec_computeNgram_asm.S $CODE_SRC/vec_majority_3_asm.S $CODE_SRC/AM_v.S $CHIPYARD_MAIN/generators/esp-tests/benchmarks/common/syscalls.c $CHIPYARD_MAIN/generators/esp-tests/benchmarks/common/crt.S -o main_vec-baremetal  -DPREALLOCATE=1 -DMULTITHREAD=1 -mcmodel=medany -std=gnu99 -O2 -ffast-math -fno-common -fno-builtin-printf -march=rv64gcxhwacha -lm -lgcc -I$CHIPYARD_MAIN/generators/esp-tests -I$CHIPYARD_MAIN/generators/esp-tests/env -I$CODE_SRC -I$CHIPYARD_MAIN/generators/HD_sensor_fusion/software -I$CHIPYARD_MAIN/generators/esp-tests/benchmarks/common -DID_STRING=  -nostdlib -nostartfiles -static -T $CHIPYARD_MAIN/generators/esp-tests/benchmarks/common/test.ld -DBAREMETAL=1
```

## Run Simulations
On SPICE servers, you'll need to

```
source /ecad/tools/vlsi.bashrc
```

before you can use CAD software. After this has been done, you are now set to run the simulations using the following command:
```
cd $VCS_SRC
make run-binary CONFIG=HwachaRocketConfig BINARY=$CODE_SRC/main_vec-baremetal
```