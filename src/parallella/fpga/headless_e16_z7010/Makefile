include ../quiet.mk

M_DEPS := run.tcl
M_DEPS += ../parallella_base/parallella_base.xpr
M_DEPS += ../parallella_base/run.tcl

M_VIVADO := vivado -mode batch -source

M_FLIST := *.cache
M_FLIST += *.data
M_FLIST += *.xpr
M_FLIST += *.log
M_FLIST += *.jou
M_FLIST +=  xgui
M_FLIST += *.runs
M_FLIST += *.srcs
M_FLIST += *.sdk
M_FLIST += .Xil
M_FLIST += parallella_e16_headless_gpiose_7010.bit.bin
M_FLIST += system_wrapper.bit.bin

.PHONY: all clean

build_steps = \
	$(M_VIVADO) run.tcl && \
	bootgen -arch zynq -w on -image bit2bin.bif -split bin && \
	cp -f system_wrapper.bit.bin parallella_e16_headless_gpiose_7010.bit.bin

all: $(M_DEPS)
	$(call build,($(build_steps)),build.log,headless_e16_z7010)

clean:
	$(call clean,$(M_FLIST),headless_e16_z7010)
