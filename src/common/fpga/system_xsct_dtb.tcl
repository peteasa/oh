################################################################################
# XSCT / HSI tcl script for building system devicetree blob
#
# Usage:
# xsct system_xsct_dtb.tcl
################################################################################

set path_sdk ./sdk
set design system

set ver 2024.2
foreach item $argv {
  puts "Input argument: $item"
  if {[lsearch -all $item "*DTG_VER*"] >= 0} {
    set param [split $item "="]
    if {[lindex $param 1] ne ""} {
      set ver [lindex $param 1]
    }
  }

  if {[lsearch -all $item "dtg_path*"] >= 0} {
    set param [split $item "="]
    if {[lindex $param 1] ne ""} {
      set dtg_path [lindex $param 1]
    }
  }

  if {[lsearch -all $item "path_sdk*"] >= 0} {
    set param [split $item "="]
    if {[lindex $param 1] ne ""} {
      set path_sdk [lindex $param 1]
    }
  }
}
puts "DTG path: $dtg_path"
puts "DTG version: $ver"

hsi open_hw_design $path_sdk/$design.xsa

#file mkdir $path_sdk
catch {cd $path_sdk}

hsi set_repo_path $dtg_path/device-tree-xlnx-xilinx-v$ver/

hsi create_sw_design device-tree -os device_tree -proc ps7_cortexa9_0

hsi set_property CONFIG.kernel_version $ver [hsi get_os]
#hsi set_property CONFIG.dt_overlay true [hsi get_os]

hsi generate_target -dir dts

exit
