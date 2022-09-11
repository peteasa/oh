include ../quiet.mk

M_DEPS := run.tcl

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

all: $(M_DEPS)
	$(call build,$(M_VIVADO) run.tcl,build.log,parallella_accelerator)

clean:
	$(call clean,$(M_FLIST),parallella_accelarator)
