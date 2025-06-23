`include "elink_regmap.vh"
module dut(/*AUTOARG*/
   // Outputs
   dut_active, clkout, wait_out, access_out, packet_out,
   // Inputs
   clk1, clk2, nreset, vdd, vss, access_in, packet_in, wait_in
   );

   //##########################################################################
   //# INTERFACE
   //##########################################################################

   parameter AW    = 32;
   parameter DW    = 32;
   parameter ID    = 12'h810;
   parameter S_IDW = 12;
   parameter M_IDW = 6;
   parameter PW    = 2*AW + 40;
   parameter N     = 1;
   parameter RETURN_ADDR = {ID,
                            `EGROUP_RR,
                            16'b0};      // axi return addr
   localparam STRBW = DW / 8;

   //clock,reset
   input             clk1;
   input             clk2;
   input             nreset;
   input [N*N-1:0]   vdd;
   input             vss;
   output            dut_active;
   output            clkout;

   //Stimulus Driven Transaction
   input [N-1:0]     access_in;
   input [N*PW-1:0]  packet_in;
   output [N-1:0]    wait_out;

   //DUT driven transaction
   output [N-1:0]    access_out;
   output [N*PW-1:0] packet_out;
   input [N-1:0]     wait_in;

   //##########################################################################
   //#BODY
   //##########################################################################

   reg               dut_active;

   wire              read_in;
   wire              write_in;

   wire [M_IDW-1:0]  m_axi_wid;
   wire [M_IDW-1:0]  m_axi_awid;

   wire              wr_wait;
   wire              rd_wait;

   wire              mem_access;
   wire [PW-1:0]     mem_packet;
   wire              emem_wait;
   wire              mem_wr_access;
   wire [PW-1:0]     mem_wr_packet;
   wire              mem_wr_wait;
   wire              mem_rd_access;
   wire [PW-1:0]     mem_rd_packet;
   wire              mem_rd_wait;

   /*AUTOINPUT*/
   /*AUTOINOUT*/
   /*AUTOWIRE*/
   // Beginning of automatic wires (for undeclared instantiated-module outputs)
   wire [AW-1:0]        m_axi_araddr;           // From emaxi of emaxi.v
   wire [1:0]           m_axi_arburst;          // From emaxi of emaxi.v
   wire [3:0]           m_axi_arcache;          // From emaxi of emaxi.v
   wire [M_IDW-1:0]     m_axi_arid;             // From emaxi of emaxi.v
   wire [7:0]           m_axi_arlen;            // From emaxi of emaxi.v
   wire                 m_axi_arlock;           // From emaxi of emaxi.v
   wire [2:0]           m_axi_arprot;           // From emaxi of emaxi.v
   wire [3:0]           m_axi_arqos;            // From emaxi of emaxi.v
   wire                 m_axi_arready;          // From esaxi of esaxi.v
   wire [2:0]           m_axi_arsize;           // From emaxi of emaxi.v
   wire                 m_axi_arvalid;          // From emaxi of emaxi.v
   wire [AW-1:0]        m_axi_awaddr;           // From emaxi of emaxi.v
   wire [1:0]           m_axi_awburst;          // From emaxi of emaxi.v
   wire [3:0]           m_axi_awcache;          // From emaxi of emaxi.v
   wire [7:0]           m_axi_awlen;            // From emaxi of emaxi.v
   wire                 m_axi_awlock;           // From emaxi of emaxi.v
   wire [2:0]           m_axi_awprot;           // From emaxi of emaxi.v
   wire [3:0]           m_axi_awqos;            // From emaxi of emaxi.v
   wire                 m_axi_awready;          // From esaxi of esaxi.v
   wire [2:0]           m_axi_awsize;           // From emaxi of emaxi.v
   wire                 m_axi_awvalid;          // From emaxi of emaxi.v
   wire [S_IDW-1:0]     m_axi_bid;              // From esaxi of esaxi.v
   wire                 m_axi_bready;           // From emaxi of emaxi.v
   wire [1:0]           m_axi_bresp;            // From esaxi of esaxi.v
   wire                 m_axi_bvalid;           // From esaxi of esaxi.v
   wire [31:0]          m_axi_rdata;            // From esaxi of esaxi.v
   wire [S_IDW-1:0]     m_axi_rid;              // From esaxi of esaxi.v
   wire                 m_axi_rlast;            // From esaxi of esaxi.v
   wire                 m_axi_rready;           // From emaxi of emaxi.v
   wire [1:0]           m_axi_rresp;            // From esaxi of esaxi.v
   wire                 m_axi_rvalid;           // From esaxi of esaxi.v
   wire [63:0]          m_axi_wdata;            // From emaxi of emaxi.v
   wire                 m_axi_wlast;            // From emaxi of emaxi.v
   wire                 m_axi_wready;           // From esaxi of esaxi.v
   wire [7:0]           m_axi_wstrb;            // From emaxi of emaxi.v
   wire                 m_axi_wvalid;           // From emaxi of emaxi.v
   wire                 mem_rr_access;          // From ememory of ememory.v
   wire [PW-1:0]        mem_rr_packet;          // From ememory of ememory.v
   wire                 mem_rr_wait;            // From esaxi of esaxi.v
   // End of automatics

   assign clkout = clk1;

   always @ (posedge clk1 or negedge nreset)
     if(!nreset)
       dut_active <= 1'b0;
     else if(dut_active | m_axi_wstrb[0])
       dut_active <= 1'b1;


   //######################################################################
   //AXI MASTER
   //######################################################################

   //Split stimulus to read/write
   assign wait_out = (wr_wait & packet_in[0]) | (rd_wait & ~packet_in[0]);
   assign write_in = access_in & packet_in[0];
   assign read_in  = access_in & ~packet_in[0];

   emaxi #(.M_IDW(M_IDW))
   emaxi (// Inputs
          .m_axi_aclk                   (clk1),
          .m_axi_aresetn                (nreset),
          .wr_access                    (write_in),
          .wr_packet                    (packet_in[PW-1:0]),
          .rd_access                    (read_in),
          .rd_packet                    (packet_in[PW-1:0]),
          .rr_wait                      (wait_in),
          // Outputs
          .wr_wait                      (wr_wait),
          .rd_wait                      (rd_wait),
          .rr_access                    (access_out),
          .rr_packet                    (packet_out[PW-1:0]),
          /*AUTOINST*/
          // Outputs
          .m_axi_awid                   (m_axi_awid[M_IDW-1:0]),
          .m_axi_awaddr                 (m_axi_awaddr[AW-1:0]),
          .m_axi_awlen                  (m_axi_awlen[7:0]),
          .m_axi_awsize                 (m_axi_awsize[2:0]),
          .m_axi_awburst                (m_axi_awburst[1:0]),
          .m_axi_awlock                 (m_axi_awlock),
          .m_axi_awcache                (m_axi_awcache[3:0]),
          .m_axi_awprot                 (m_axi_awprot[2:0]),
          .m_axi_awqos                  (m_axi_awqos[3:0]),
          .m_axi_awvalid                (m_axi_awvalid),
          .m_axi_wid                    (m_axi_wid[M_IDW-1:0]),
          .m_axi_wdata                  (m_axi_wdata[63:0]),
          .m_axi_wstrb                  (m_axi_wstrb[7:0]),
          .m_axi_wlast                  (m_axi_wlast),
          .m_axi_wvalid                 (m_axi_wvalid),
          .m_axi_bready                 (m_axi_bready),
          .m_axi_arid                   (m_axi_arid[M_IDW-1:0]),
          .m_axi_araddr                 (m_axi_araddr[AW-1:0]),
          .m_axi_arlen                  (m_axi_arlen[7:0]),
          .m_axi_arsize                 (m_axi_arsize[2:0]),
          .m_axi_arburst                (m_axi_arburst[1:0]),
          .m_axi_arlock                 (m_axi_arlock),
          .m_axi_arcache                (m_axi_arcache[3:0]),
          .m_axi_arprot                 (m_axi_arprot[2:0]),
          .m_axi_arqos                  (m_axi_arqos[3:0]),
          .m_axi_arvalid                (m_axi_arvalid),
          .m_axi_rready                 (m_axi_rready),
          // Inputs
          .m_axi_awready                (m_axi_awready),
          .m_axi_wready                 (m_axi_wready),
          .m_axi_bid                    (m_axi_bid[M_IDW-1:0]),
          .m_axi_bresp                  (m_axi_bresp[1:0]),
          .m_axi_bvalid                 (m_axi_bvalid),
          .m_axi_arready                (m_axi_arready),
          .m_axi_rid                    (m_axi_rid[M_IDW-1:0]),
          .m_axi_rdata                  (m_axi_rdata[63:0]),
          .m_axi_rresp                  (m_axi_rresp[1:0]),
          .m_axi_rlast                  (m_axi_rlast),
          .m_axi_rvalid                 (m_axi_rvalid));

   /*esaxi AUTO_TEMPLATE (//Stimulus
    .s_\(.*\)     (m_\1[]),
    .\(.*\)       (mem_\1[]),
    );
    */

   esaxi #(.S_IDW(S_IDW), .RETURN_ADDR(RETURN_ADDR))
   esaxi (// Inputs
          .s_axi_aclk                   (clk1),
          .s_axi_aresetn                (nreset),
          .s_axi_awid                   ({(S_IDW){1'b0}}),
          .s_axi_wid                    ({(S_IDW){1'b0}}),
          .s_axi_arid                   ({(S_IDW){1'b0}}),
          .s_axi_wstrb                  ((m_axi_wstrb[3:0] | m_axi_wstrb[7:4])),//NOTE:HACK!!
          // Outputs
          /*AUTOINST*/
          // Outputs
          .wr_access                    (mem_wr_access),         // Templated
          .wr_packet                    (mem_wr_packet[PW-1:0]), // Templated
          .rd_access                    (mem_rd_access),         // Templated
          .rd_packet                    (mem_rd_packet[PW-1:0]), // Templated
          .rr_wait                      (mem_rr_wait),           // Templated
          .s_axi_arready                (m_axi_arready),         // Templated
          .s_axi_awready                (m_axi_awready),         // Templated
          .s_axi_bid                    (m_axi_bid[S_IDW-1:0]),  // Templated
          .s_axi_bresp                  (m_axi_bresp[1:0]),      // Templated
          .s_axi_bvalid                 (m_axi_bvalid),          // Templated
          .s_axi_rid                    (m_axi_rid[S_IDW-1:0]),  // Templated
          .s_axi_rdata                  (m_axi_rdata[31:0]),     // Templated
          .s_axi_rlast                  (m_axi_rlast),           // Templated
          .s_axi_rresp                  (m_axi_rresp[1:0]),      // Templated
          .s_axi_rvalid                 (m_axi_rvalid),          // Templated
          .s_axi_wready                 (m_axi_wready),          // Templated
          // Inputs
          .wr_wait                      (mem_wr_wait),           // Templated
          .rd_wait                      (mem_rd_wait),           // Templated
          .rr_access                    (mem_rr_access),         // Templated
          .rr_packet                    (mem_rr_packet[PW-1:0]), // Templated
          .s_axi_araddr                 (m_axi_araddr[AW-1:0]),  // Templated
          .s_axi_arburst                (m_axi_arburst[1:0]),    // Templated
          .s_axi_arcache                (m_axi_arcache[3:0]),    // Templated
          .s_axi_arlock                 (m_axi_arlock),          // Templated
          .s_axi_arlen                  (m_axi_arlen[7:0]),      // Templated
          .s_axi_arprot                 (m_axi_arprot[2:0]),     // Templated
          .s_axi_arqos                  (m_axi_arqos[3:0]),      // Templated
          .s_axi_arsize                 (m_axi_arsize[2:0]),     // Templated
          .s_axi_arvalid                (m_axi_arvalid),         // Templated
          .s_axi_awaddr                 (m_axi_awaddr[AW-1:0]),  // Templated
          .s_axi_awburst                (m_axi_awburst[1:0]),    // Templated
          .s_axi_awcache                (m_axi_awcache[3:0]),    // Templated
          .s_axi_awlock                 (m_axi_awlock),          // Templated
          .s_axi_awlen                  (m_axi_awlen[7:0]),      // Templated
          .s_axi_awprot                 (m_axi_awprot[2:0]),     // Templated
          .s_axi_awqos                  (m_axi_awqos[3:0]),      // Templated
          .s_axi_awsize                 (m_axi_awsize[2:0]),     // Templated
          .s_axi_awvalid                (m_axi_awvalid),         // Templated
          .s_axi_bready                 (m_axi_bready),          // Templated
          .s_axi_rready                 (m_axi_rready),          // Templated
          .s_axi_wdata                  (m_axi_wdata[31:0]),     // Templated
          .s_axi_wlast                  (m_axi_wlast),           // Templated
          .s_axi_wvalid                 (m_axi_wvalid));         // Templated

   //######################################################################
   // MEMORY PORT
   //######################################################################

   //"Arbitration" between read/write transaction
   assign mem_access           = mem_wr_access | mem_rd_access;

   assign mem_packet[PW-1:0]   = mem_wr_access ? mem_wr_packet[PW-1:0]:
                                 mem_rd_packet[PW-1:0];

   assign mem_rd_wait      = (emem_wait & mem_rd_access) |
                             mem_wr_access;

   assign mem_wr_wait      = (emem_wait & mem_wr_access);

   /*ememory AUTO_TEMPLATE (
                        // Outputsd
                        .\(.*\)_out       (mem_rr_\1[]),
                        .\(.*\)_in        (mem_\1[]),
                        .wait_out      (mem_wait),
                             );
   */

   ememory #(.WAIT(0),
             .MON(1))
   ememory (// Inputs
            .wait_in            (mem_rr_wait),//pushback on reads
            .clk                (clk1),
            .coreid             (12'h0),
            // Outputs
            .wait_out           (emem_wait),
            /*AUTOINST*/
            // Outputs
            .access_out                 (mem_rr_access),         // Templated
            .packet_out                 (mem_rr_packet[PW-1:0]), // Templated
            // Inputs
            .nreset                     (nreset),
            .access_in                  (mem_access),            // Templated
            .packet_in                  (mem_packet[PW-1:0]));   // Templated

   endmodule // dv_axi
// Local Variables:
// verilog-library-directories:("." "../hdl" "../../emesh/dv" "../../axi/dv" "../../emesh/hdl" "../../axi/hdl")
// End:
