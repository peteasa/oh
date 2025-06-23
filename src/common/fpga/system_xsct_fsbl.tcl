################################################################################
# XSCT / HSI tcl script for building system fsbl
#
# Usage:
# xsct system_xsct_fsbl.tcl
################################################################################

set design system
set path_sdk ./sdk

foreach item $argv {
  puts "Input argument: $item"

  if {[lsearch -all $item "path_sdk*"] >= 0} {
    set param [split $item "="]
    if {[lindex $param 1] ne ""} {
      set path_sdk [lindex $param 1]
    }
  }
}

hsi open_hw_design $path_sdk/$design.xsa

catch {cd $path_sdk}

hsi generate_app -os standalone -proc ps7_cortexa9_0 -app zynq_fsbl -compile -sw fsbl -dir fsbl

exit
