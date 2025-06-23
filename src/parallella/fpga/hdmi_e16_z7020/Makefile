include ../quiet.mk

MACHINE	    := parallella_e16_hdmi_gpiose_7020
FPGA_BIN    := $(MACHINE).bit.bin

FPGA_DTB    := $(MACHINE).dtb
FPGA_DTS    := $(MACHINE).dts
DTS_DIR     := sdk/dts
EDTS_DIR    := ../elink_dts
VITIS_VER   := 2024.2
DTG_PATH    := $(OH_HOME)/..
SCRIPT_HOME := $(OH_HOME)/src/common/fpga

M_DEPS := run.tcl
M_DEPS += ../parallella_base/parallella_base.xpr
M_DEPS += ../parallella_base/run.tcl

M_VIVADO := vivado -mode batch -source

M_FLIST := reports
M_FLIST += results
M_FLIST += *.cache
M_FLIST += *.data
M_FLIST += *.xpr
M_FLIST += *.log
M_FLIST += *.jou
M_FLIST +=  xgui
M_FLIST += *.gen
M_FLIST += *.hw
M_FLIST += *.hdf
M_FLIST += *.hwdef
M_FLIST += *.ip_user_files
M_FLIST += *.runs
M_FLIST += *.srcs
M_FLIST += *.sim
M_FLIST += *.sdk
M_FLIST += .Xil
M_FLIST += $(FPGA_BIN)
M_FLIST += system_wrapper.bit.bin
M_FLIST += *.bin
M_FLIST += *.bit
M_FLIST += sdk
M_FLIST += $(FPGA_DTB)
M_FLIST += $(FPGA_DTS)

.PHONY: all clean

build_steps = \
	$(M_VIVADO) run.tcl && \
	bootgen -arch zynq -w on -image bit2bin.bif -split bin && \
	cp -f system_wrapper.bit.bin $(FPGA_BIN)

all: $(FPGA_BIN) $(FPGA_DTB)

$(FPGA_BIN): $(M_DEPS)
	$(call build,($(build_steps)),build.log,hdmi_e16_z7020)

$(FPGA_DTB): $(FPGA_BIN)
	xsct $(SCRIPT_HOME)/system_xsct_dtb.tcl dtg_path=$(DTG_PATH) DTG_VER=$(VITIS_VER)
	# vitis -s vitis_create_fsbl.py $(LOG)

    # OPTIONAL: fix bug with #address-cells and #size-cells for overlay
	#sed -i 's/#address-cells/\/\/#address-cells/g' $(DTS_DIR)/pl.dtsi
	#sed -i 's/#size-cells/\/\/#size-cells/g' $(DTS_DIR)/pl.dtsi
	#sed -i 's/red_pitaya.bit.bin/fpga.bit.bin/g' $(DTS_DIR)/pl.dtsi
	#sed -i 's/#include "pl.dtsi"//' $(DTS_DIR)/system-top.dts
	echo '#include "zynq-para-base.dtsi"' >> $(DTS_DIR)/system-top.dts
	echo '#include "zynq-para-elink.dtsi"' >> $(DTS_DIR)/system-top.dts
	echo '#include "zynq-para-hdmi.dtsi"' >> $(DTS_DIR)/system-top.dts

	gcc -E -nostdinc -undef -D__DTS__ -x assembler-with-cpp -I $(EDTS_DIR) -I $(DTS_DIR) -o $(DTS_DIR)/system-top.dtsi.pp $(DTS_DIR)/system-top.dts
	dtc -@ -O dtb -o $(FPGA_DTB) -i $(DTS_DIR) -i $(EDTS_DIR) $(DTS_DIR)/system-top.dtsi.pp
	dtc -I dtb -O dts $(FPGA_DTB) -o $(FPGA_DTS)

clean:
	$(call clean,$(M_FLIST),hdmi_e16_z7020)
