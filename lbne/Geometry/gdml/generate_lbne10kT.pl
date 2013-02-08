#!/usr/bin/perl

# Much of this program is taken straight from generate_gdml.pl that 
# generates MicroBooNE fragment files (Thank you.)

# Each subroutine generates a fragment GDML file, and the last subroutine
# creates an XML file that make_gdml.pl will use to appropriately arrange
# the fragment GDML files to create the final desired LBNE GDML file, 
# to be named by make_gdml output command

# If you are playing with different geometries, you can use the
# suffix command to help organize your work.

use Math::Trig;
use Getopt::Long;
use Math::BigFloat;
Math::BigFloat->precision(-10);

GetOptions( "help|h" => \$help,
	    "suffix|s:s" => \$suffix,
	    "output|o:s" => \$output,
	    "wires|w:s" => \$wires,
            "helpcube|c" => \$helpcube);

if ( defined $help )
{
    # If the user requested help, print the usage notes and exit.
    usage();
    exit;
}

if ( ! defined $suffix )
{
    # The user didn't supply a suffix, so append nothing to the file
    # names.
    $suffix = "";
}
else
{
    # Otherwise, stick a "-" before the suffix, so that a suffix of
    # "test" applied to filename.gdml becomes "filename-test.gdml".
    $suffix = "-" . $suffix;
}


#++++++++++++++++++++++++ Begin defining variables +++++++++++++++++++++++++

# Define detector geometry variables - later to be put in a parameters
# XML file to be parsed as an input?

# set wires on to be the default, unless given an input by the user
$wires_on = 1; # 1=on, 0=off
if (defined $wires)
{
$wires_on = $wires
}

$tpc_on=1;

# wire plane parameters
$UWirePitch = .49;
$VWirePitch = .5;
$XWirePitch = .45;

$UAngle = 45.7;
$VAngle = 44.3;

$SinUAngle = sin( deg2rad($UAngle) );
$CosUAngle = cos( deg2rad($UAngle) );
$TanUAngle = tan( deg2rad($UAngle) );

$SinVAngle = sin( deg2rad($VAngle) );
$CosVAngle = cos( deg2rad($VAngle) );
$TanVAngle = tan( deg2rad($VAngle) );

$UWire_yint = $UWirePitch/$SinUAngle;
$UWire_zint = $UWirePitch/$CosUAngle;

$VWire_yint = $VWirePitch/$SinVAngle;
$VWire_zint = $VWirePitch/$CosVAngle;

# APA Parameters
$nCryos	         = 2;
$nAPAWide		= 3; 
$nAPAHigh		= 2;
$nAPALong		= 10;

#$APAFrameWidth = 8.9; #this includes the width of all of the wire planes (g, u,v, and x)
$CPAThickness = 0.002; #for now the CPA is basically infinitely thin
$APAFrameWidth = 5.08; #this does not include the wire spacing
$APAWirePlaneSpacing = 0.476; #spacing between all of the wire planes (g, u, v, and x)
$MaxDrift = 233.555; #this is the distance form the CPA to the gage wire plane and designed to have APA-CPA spacing at 2.38m

$APALongGap = 1.5; #this is the separation between APAs along the incident beam axis
$APAVerticalGap = 2.5; #this is the separation between APAs along the vertical axis 

$APAWidth   =   $CPAThickness + 2*$MaxDrift + 8*$APAWirePlaneSpacing + $APAFrameWidth ; # this is the distance center-line CPA to center-line CPA
$APAHeight  =   700; #does not include front-end boards or hanger posts
$APALength  =   252; #does not include the dead space on the sides of APA frame for wrapping wires

$FiducialWidth	       	=	$APAWidth*$nAPAWide; #includes the APA frame (gage wire to gage wire)
$FiducialHeight		=	$APAHeight*$nAPAHigh + ($nAPAHigh - 1)*$APAVerticalGap; #includes dead space between APA frames
$FiducialLength		=	$APALength*$nAPALong + ($nAPALong - 1)*$APALongGap; #includes dead space between APA frames

#Cryostat space with LAr outside of entire TPCdetector
$SpaceCPAtoCryoWall=85; #$SideLArPadding
$SpaceAPAToFloor=50; #$BottomLArPadding
$SpaceAPAToTopLAr=50; #$TopLArPadding
$HeightGaseousAr = 50; #$Height of Gaseous Ar region
$UpstreamLArPadding=50;
$DownstreamLArPadding=250;

$SteelThickness		=	0.5*2.54; #half inch
$ArgonWidth		=	$FiducialWidth+2*$SpaceCPAToCryoWall;
$ArgonHeight		=	$FiducialHeight+$SpaceAPAToFloor+$SpaceAPAToTopLAr;
$ArgonLength		=	$FiducialLength+$UpstreamLArPadding+$DownstreamLArPadding;

$CryostatWidth		=	$ArgonWidth+2*$SteelThickness;
$CryostatHeight		=	$ArgonHeight+2*$SteelThickness + $HeightGaseousAr;
$CryostatLength		=	$ArgonLength+2*$SteelThickness; #this should also have twice the steel thickness, yes?

$TPCWidth		 =	($APAWidth-$APAFrameWidth - $CPAThicknes/2)/2 - 0.001; # leaving space for frame, this distance is the distance from edge of the APA frame to the center of the CPA
$TPCHeight		 =	$APAHeight - 0.001 ; #active volume only and doesn't include front-end boards or dead space between two vertically stacked APAs
$TPCLength		 =	$APALength - 0.001 ; # active volume only and doesn't include dead space between adjacent APAs

$TPCWirePlaneThickness   =   .01;
$TPCWirePlaneHeight   =   $TPCHeight; #the wire plane region is the full height of the APA since the previous number doesn't have the front-end boards, etc.
$TPCWirePlaneLength   =   $TPCLength; #the APA Length doesn't have the spacing on between the two APAs so the Wire Plane Length is the full length

$TPCWireThickness=0.015;


$ArToAr=300;  # x distance between the LAr in each side by side detector
$ConcretePadding      =	50;
$FoamPadding          = 80;
$TotalPadding	      =	$ConcretePadding+$FoamPadding;
$DetEncWidth	      =	2*$CryostatWidth+2*$TotalPadding+2*$FoamPadding + $ArToAr;
$DetEncHeight	      =	$CryostatHeight+$ConcretePadding; #no foam on bottom or top, no concrete on top
$DetEncLength         = $CryostatLength+2*$TotalPadding;

$PosDirCubeSide = 0;
if (defined $helpcube)
{
$PosDirCubeSide = $ArToAr; #seems to be a good proportion
}

#TPC Active Variables
#these numbers are wrong, the APAFrameWidth of 8.9 cm already includes the wire planes
$TPCActiveWidth   =  $TPCWidth-(4*$APAWirePlaneSpacing);#last -1.5 is to temporarily fix overlap.
$TPCActiveHeight  =  $TPCWirePlaneHeight; #any cuts?
$TPCActiveLength  =  $TPCWirePlaneLength; #any cuts?

#don't understand this calculation at all
$posTPCActive_X   =  $TPCWidth/2-$TPCActiveWidth/2;     #(.5+$TPCWirePlaneThickness/2+0.3*3)/2;
$posTPCActive_Y   =  0;
$posTPCActive_Z   =  0;

####################
#Flashback to old parameters, for now so that lbne_geo.C works

$RockThickness	     =	3000;
$HighBayTopRockThickness    = 320;
$LowBayTopRockThickness    = 250;

$WorldWidth  = 3*$RockThickness;
$WorldHeight = 3*$RockThickness;
$WorldLength = 3*$RockThickness;

#$WorldWidth  = 3*$DetEncWidth;
#$WorldHeight = 3*$DetEncHeight;
#$WorldLength = 3*$DetEncLength;

$ServiceBuildingExtraWidth =    890; #for now 890 is the difference between 4244 and 3354 which are the spec'd service building width and detector enclosure width 11/1/2012

#LowBayDefinitions
$LowBayInsideWidth     =       $DetEncWidth + $ServiceBuildingExtraWidth;
$LowBayInsideHeight    =       350;
$LowBayInsideLength    =       2666;

$LowBayCeilingThickness=       2*$ConcretePadding;
$LowBaySideWallThickness =     $ConcretePadding;
$LowBayBackWallThickness =     $ConcretePadding;

#HighBayDefinitions
$HighBayInsideWidth     =       $DetEncWidth + $ServiceBuildingExtraWidth;
$HighBayInsideHeight    =       1048;
$HighBayInsideLength    =       1719;

$HighBayCeilingThickness=       2*$ConcretePadding;
$HighBayFrontWallThickness =    $ConcretePadding;
$HighBaySideWallThickness =     $ConcretePadding;
$HighBayBackWallThickness =     2*$ConcretePadding;

$HighBayOverlap         =       500; #the opening for access to the DetEncl and position of the HighBay

$GabionThickness        =       400;

$SlopeRockFillHeight    =       ($HighBayInsideHeight + $HighBayCeilingThickness +$HighBayTopRockThickness - $LowBayInsideHeight - $LowBayCeilingThickness - $LowBayTopRockThickness);
$SlopeRockFillLength    =       $SlopeRockFillHeight*2;

$HillSideAngle = 20; #in degrees
$HillSideLength = $RockThickness + $DetEncLength;
$HillSideHeight = $HillSideLength * tan( deg2rad($HillSideAngle));
$HillSideWidth = $RockThickness - ($ServiceBuildingExtraWidth/2 + $LowBaySideWallThickness + $GabionThickness);

$HillSideBoxLength = $RockThickness - ($LowBayInsideLength +$LowBayBackWallThickness + $HighBayOverlap +$HighBayBackWallThickness - $DetEncLength );
$HillSideBoxWidth = $LowBayInsideWidth + 2*$LowBaySideWallThickness + 2*$GabionThickness;
$HillSideBoxHeight = $LowBayInsideHeight +$LowBayCeilingThickness +$LowBayTopRockThickness;

$HillSideMiddleLength = $RockThickness + $DetEncLength - $HighBayOverlap - $HighBayBackWallThickness - $SlopeRockFillLength;
$HillSideMiddleHeight = $HillSideMiddleLength * tan( deg2rad($HillSideAngle));
$HillSideMiddleWidth = $HillSideBoxWidth;



$LightPaddleWidth = 0.476;
$LightPaddleHeight = 2.54*4;
$LightPaddleLength = 225-0.001;
$nLightPaddlesPerAPA = 10; #10, or 20 for double coverage (for now)
$PaddleYInterval=(2*$APAHeight+$APAVerticalGap-$LightPaddleHeight)/(2*$nLightPaddlesPerAPA-1);
$FrameToPaddleSpace = ($PaddleYInterval-$APAVerticalGap)/2;

# $PaddleYInterval is defined so that the center-to-center distance in the 
# y direction between paddles is uniform between vertically stacked APAs.
# $FrameToPaddleSpace is from the BOTTOM of the APA frame (4" in y direction)
# to the CENTER of a paddle, including the 4" part of the frame. This variable's
# primary purpose is to position the lowest paddle in each APA.


####################

#+++++++++++++++++++++++++ End defining variables ++++++++++++++++++++++++++

# run the sub routines that generate the fragments

gen_Define(); 	 # generates definitions at beginning of GDML
gen_Materials(); # generates materials to be used

gen_TPC();	 # generates wires, wire planes, and puts them in volTPC
	         # This is the bulk of the code, and has zero wires option
gen_Cryostat();	 # places (2*nAPAWide x nAPAHigh x nAPALong) volTPC,
		 # half rotated 180 about Y
gen_Enclosure(); # places two cryostats and concrete volumes

#gen_ServiceBuilding(); #puts the service building over top of the Enclosure, note that the floor of the service 
                        #building is built here but carved out of the surrounding rock

#gen_HillSide(); #puts the rock around the detector enclosure and rock fill and hillside around the service building

gen_World();	 # places the enclosure among DUSEL Rock


write_fragments(); # writes the XML input for make_gdml.pl
			# which zips together the final GDML
exit;


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++ usage +++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub usage()
{
    print "Usage: $0 [-h|--help] [-o|--output <fragments-file>] [-s|--suffix <string>]\n";
    print "       if -o is omitted, output goes to STDOUT; <fragments-file> is input to make_gdml.pl\n";
    print "       -s <string> appends the string to the file names; useful for multiple detector versions\n";
    print "       -h prints this message, then quits\n";
}



#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++++ gen_Define +++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_Define()
{

# Create the <define> fragment file name, 
# add file to list of fragments,
# and open it
    $DEF = "lbne_10kT_Def" . $suffix . ".gdml";
    push (@gdmlFiles, $DEF);
    $DEF = ">" . $DEF;
    open(DEF) or die("Could not open file $DEF for writing");


print DEF <<EOF;
<?xml version='1.0'?>
<gdml>
<define>
   <position name="posTPCActive"        unit="cm" x="$posTPCActive_X" y="$posTPCActive_Y" z="$posTPCActive_Z"/>
   <rotation name="rPlus90AboutX"       unit="deg" x="90" y="0" z="0"/>
   <rotation name="rMinus90AboutY"       unit="deg" x="0" y="270" z="0"/>
   <rotation name="rMinus90AboutYMinus90AboutX"       unit="deg" x="270" y="270" z="0"/>
   <rotation name="rPlusUAngleAboutX"	unit="deg" x="90-$UAngle" y="0"   z="0"/>
   <rotation name="rPlusVAngleAboutX"	unit="deg" x="90+$VAngle" y="0"   z="0"/>
   <rotation name="rPlus180AboutY"	unit="deg" x="0" y="180"   z="0"/>
   <rotation name="rIdentity"		unit="deg" x="0" y="0"   z="0"/>
</define>
</gdml>
EOF
    close (DEF);
}




#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++ gen_Materials +++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_Materials() 
{

# Create the <materials> fragment file name,
# add file to list of output GDML fragments,
# and open it
    $MAT = "lbne_10kT_Materials" . $suffix . ".gdml";
    push (@gdmlFiles, $MAT);
    $MAT = ">" . $MAT;
    open(MAT) or die("Could not open file $MAT for writing");


  print MAT <<EOF;
<materials>
  <element name="videRef" formula="VACUUM" Z="1">  <atom value="1"/> </element>
  <element name="bromine" formula="Br" Z="35"> <atom value="79.904"/> </element>
  <element name="hydrogen" formula="H" Z="1">  <atom value="1.0079"/> </element>
  <element name="nitrogen" formula="N" Z="7">  <atom value="14.0067"/> </element>
  <element name="oxygen" formula="O" Z="8">  <atom value="15.999"/> </element>
  <element name="aluminum" formula="Al" Z="13"> <atom value="26.9815"/>  </element>
  <element name="silicon" formula="Si" Z="14"> <atom value="28.0855"/>  </element>
  <element name="carbon" formula="C" Z="6">  <atom value="12.0107"/>  </element>
  <element name="potassium" formula="K" Z="19"> <atom value="39.0983"/>  </element>
  <element name="chromium" formula="Cr" Z="24"> <atom value="51.9961"/>  </element>
  <element name="iron" formula="Fe" Z="26"> <atom value="55.8450"/>  </element>
  <element name="nickel" formula="Ni" Z="28"> <atom value="58.6934"/>  </element>
  <element name="calcium" formula="Ca" Z="20"> <atom value="40.078"/>   </element>
  <element name="magnesium" formula="Mg" Z="12"> <atom value="24.305"/>   </element>
  <element name="sodium" formula="Na" Z="11"> <atom value="22.99"/>    </element>
  <element name="titanium" formula="Ti" Z="22"> <atom value="47.867"/>   </element>
  <element name="argon" formula="Ar" Z="18"> <atom value="39.9480"/>  </element>
  <element name="sulphur" formula="S" Z="16"> <atom value="32.065"/>  </element>
  <element name="phosphorus" formula="P" Z="16"> <atom value="30.973"/>  </element>

  <material name="Vacuum" formula="Vacuum">
   <D value="1.e-25" unit="g/cm3"/>
   <fraction n="1.0" ref="videRef"/>
  </material>

  <material name="ALUMINUM_Al" formula="ALUMINUM_Al">
   <D value="2.6990" unit="g/cm3"/>
   <fraction n="1.0000" ref="aluminum"/>
  </material>

  <material name="SILICON_Si" formula="SILICON_Si">
   <D value="2.3300" unit="g/cm3"/>
   <fraction n="1.0000" ref="silicon"/>
  </material>

  <material name="epoxy_resin" formula="C38H40O6Br4">
   <D value="1.1250" unit="g/cm3"/>
   <composite n="38" ref="carbon"/>
   <composite n="40" ref="hydrogen"/>
   <composite n="6" ref="oxygen"/>
   <composite n="4" ref="bromine"/>
  </material>

  <material name="SiO2" formula="SiO2">
   <D value="2.2" unit="g/cm3"/>
   <composite n="1" ref="silicon"/>
   <composite n="2" ref="oxygen"/>
  </material>

  <material name="Al2O3" formula="Al2O3">
   <D value="3.97" unit="g/cm3"/>
   <composite n="2" ref="aluminum"/>
   <composite n="3" ref="oxygen"/>
  </material>

  <material name="Fe2O3" formula="Fe2O3">
   <D value="5.24" unit="g/cm3"/>
   <composite n="2" ref="iron"/>
   <composite n="3" ref="oxygen"/>
  </material>

  <material name="CaO" formula="CaO">
   <D value="3.35" unit="g/cm3"/>
   <composite n="1" ref="calcium"/>
   <composite n="1" ref="oxygen"/>
  </material>

  <material name="MgO" formula="MgO">
   <D value="3.58" unit="g/cm3"/>
   <composite n="1" ref="magnesium"/>
   <composite n="1" ref="oxygen"/>
  </material>

  <material name="Na2O" formula="Na2O">
   <D value="2.27" unit="g/cm3"/>
   <composite n="2" ref="sodium"/>
   <composite n="1" ref="oxygen"/>
  </material>

  <material name="TiO2" formula="TiO2">
   <D value="4.23" unit="g/cm3"/>
   <composite n="1" ref="titanium"/>
   <composite n="2" ref="oxygen"/>
  </material>

  <material name="FeO" formula="FeO">
   <D value="5.745" unit="g/cm3"/>
   <composite n="1" ref="iron"/>
   <composite n="1" ref="oxygen"/>
  </material>

  <material name="CO2" formula="CO2">
   <D value="1.562" unit="g/cm3"/>
   <composite n="1" ref="iron"/>
   <composite n="2" ref="oxygen"/>
  </material>

  <material name="P2O5" formula="P2O5">
   <D value="1.562" unit="g/cm3"/>
   <composite n="2" ref="phosphorus"/>
   <composite n="5" ref="oxygen"/>
  </material>

  <material formula=" " name="DUSEL_Rock">
    <D value="2.82" unit="g/cc"/>
    <fraction n="0.5267" ref="SiO2"/>
    <fraction n="0.1174" ref="FeO"/>
    <fraction n="0.1025" ref="Al2O3"/>
    <fraction n="0.0473" ref="MgO"/>
    <fraction n="0.0422" ref="CO2"/>
    <fraction n="0.0382" ref="CaO"/>
    <fraction n="0.0240" ref="carbon"/>
    <fraction n="0.0186" ref="sulphur"/>
    <fraction n="0.0053" ref="Na2O"/>
    <fraction n="0.00070" ref="P2O5"/>
    <fraction n="0.0771" ref="oxygen"/>
  </material> 

  <material name="fibrous_glass">
   <D value="2.74351" unit="g/cm3"/>
   <fraction n="0.600" ref="SiO2"/>
   <fraction n="0.118" ref="Al2O3"/>
   <fraction n="0.001" ref="Fe2O3"/>
   <fraction n="0.224" ref="CaO"/>
   <fraction n="0.034" ref="MgO"/>
   <fraction n="0.010" ref="Na2O"/>
   <fraction n="0.013" ref="TiO2"/>
  </material>

  <material name="FR4">
   <D value="1.98281" unit="g/cm3"/>
   <fraction n="0.47" ref="epoxy_resin"/>
   <fraction n="0.53" ref="fibrous_glass"/>
  </material>

  <material name="STEEL_STAINLESS_Fe7Cr2Ni" formula="STEEL_STAINLESS_Fe7Cr2Ni">
   <D value="7.9300" unit="g/cm3"/>
   <fraction n="0.0010" ref="carbon"/>
   <fraction n="0.1792" ref="chromium"/>
   <fraction n="0.7298" ref="iron"/>
   <fraction n="0.0900" ref="nickel"/>
  </material>

  <material name="LAr" formula="LAr">
   <D value="1.40" unit="g/cm3"/>
   <fraction n="1.0000" ref="argon"/>
  </material>

  <material name="ArGas" formula="ArGas">
   <D value="0.00166" unit="g/cc"/>
   <fraction n="1.0" ref="argon"/>
  </material>

  <material formula=" " name="Air">
   <D value="0.001205" unit="g/cc"/>
   <fraction n="0.781154" ref="nitrogen"/>
   <fraction n="0.209476" ref="oxygen"/>
   <fraction n="0.00934" ref="argon"/>
  </material>

  <material formula=" " name="G10">
   <D value="1.7" unit="g/cc"/>
   <fraction n="0.2805" ref="silicon"/>
   <fraction n="0.3954" ref="oxygen"/>
   <fraction n="0.2990" ref="carbon"/>
   <fraction n="0.0251" ref="hydrogen"/>
  </material>

  <material formula=" " name="Granite">
   <D value="2.7" unit="g/cc"/>
   <fraction n="0.438" ref="oxygen"/>
   <fraction n="0.257" ref="silicon"/>
   <fraction n="0.222" ref="sodium"/>
   <fraction n="0.049" ref="aluminum"/>
   <fraction n="0.019" ref="iron"/>
   <fraction n="0.015" ref="potassium"/>
  </material>

  <material formula=" " name="ShotRock">
   <D value="2.7*0.6" unit="g/cc"/>
   <fraction n="0.438" ref="oxygen"/>
   <fraction n="0.257" ref="silicon"/>
   <fraction n="0.222" ref="sodium"/>
   <fraction n="0.049" ref="aluminum"/>
   <fraction n="0.019" ref="iron"/>
   <fraction n="0.015" ref="potassium"/>
  </material>

  <material formula=" " name="Dirt">
   <D value="1.7" unit="g/cc"/>
   <fraction n="0.438" ref="oxygen"/>
   <fraction n="0.257" ref="silicon"/>
   <fraction n="0.222" ref="sodium"/>
   <fraction n="0.049" ref="aluminum"/>
   <fraction n="0.019" ref="iron"/>
   <fraction n="0.015" ref="potassium"/>
  </material>

  <material formula=" " name="Concrete">
   <D value="2.3" unit="g/cc"/>
   <fraction n="0.530" ref="oxygen"/>
   <fraction n="0.335" ref="silicon"/>
   <fraction n="0.060" ref="calcium"/>
   <fraction n="0.015" ref="sodium"/>
   <fraction n="0.020" ref="iron"/>
   <fraction n="0.040" ref="aluminum"/>
  </material>

  <material formula="H2O" name="Water">
   <D value="1.0" unit="g/cc"/>
   <fraction n="0.1119" ref="hydrogen"/>
   <fraction n="0.8881" ref="oxygen"/>
  </material>

  <material formula="Ti" name="Titanium">
   <D value="4.506" unit="g/cc"/>
   <fraction n="1." ref="titanium"/>
  </material>

  <material name="TPB" formula="TPB">
   <D value="1.40" unit="g/cm3"/>
   <fraction n="1.0000" ref="argon"/>
  </material>

  <material name="Glass">
   <D value="2.74351" unit="g/cm3"/>
   <fraction n="0.600" ref="SiO2"/>
   <fraction n="0.118" ref="Al2O3"/>
   <fraction n="0.001" ref="Fe2O3"/>
   <fraction n="0.224" ref="CaO"/>
   <fraction n="0.034" ref="MgO"/>
   <fraction n="0.010" ref="Na2O"/>
   <fraction n="0.013" ref="TiO2"/>
  </material>

  <material name="Acrylic">
   <D value="1.19" unit="g/cm3"/>
   <fraction n="0.600" ref="carbon"/>
   <fraction n="0.320" ref="oxygen"/>
   <fraction n="0.080" ref="hydrogen"/>
  </material>

</materials>
EOF

close(MAT);
}




#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++++++ gen_TPC ++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


sub gen_TPC()
{

#constructs everything inside volTPC, namely
# (moving from left to right, or from +x to -x)
#  -volCathode
#  -volTPCPlaneU: with wires angled from vertical slightly different than in V
#  -volTPCPlaneV: with wires angled from vertical slightly differently than in U
#  -volTPCPlaneX: with vertical wires


# Create the TPC fragment file name,
# add file to list of output GDML fragments,
# and open it
    $TPC = "lbne_10kT_TPC" . $suffix . ".gdml";
    push (@gdmlFiles, $TPC);
    $TPC = ">" . $TPC;
    open(TPC) or die("Could not open file $TPC for writing");


# The standard XML prefix and starting the gdml
    print TPC <<EOF;
<?xml version='1.0'?>
<gdml>
EOF


# All the TPC solids save the wires.
print TPC <<EOF;
<solids>
    <box name="TPC" lunit="cm" 
      x="$TPCWidth" 
      y="$TPCHeight" 
      z="$TPCLength"/>
    <box name="Cathode" lunit="cm"
      x="1"
      y="$TPCHeight"
      z="$TPCLength"/>
    <box name="TPCPlane" lunit="cm" 
      x="$TPCWirePlaneThickness" 
      y="$TPCWirePlaneHeight" 
      z="$TPCWirePlaneLength"/>
    <box name="TPCActive" lunit="cm"
      x="$TPCActiveWidth"
      y="$TPCActiveHeight"
      z="$TPCActiveLength"/>
EOF


#++++++++++++++++++++++++++++ Wire Solids ++++++++++++++++++++++++++++++

print TPC <<EOF;

    <tube name="TPCWireVert"
      rmax="0.5*$TPCWireThickness"
      z="$TPCWirePlaneHeight"               
      deltaphi="360"
      aunit="deg"
      lunit="cm"/>
EOF

# Set number of wires to default to zero, when $wires_on = 0, for a low memory 
# version. But if $wires_on = 1, calculate the number of wires on each side of each
# plane to be used in the for loops

my $NumberCornerUWires = 0;
my $NumberSideUWires = 0;
my $NumberCommonUWires = 0;
my $NumberCornerVWires = 0;
my $NumberSideVWires = 0;
my $NumberCommonVWires = 0;
my $NumberVerticalWires = 0;

if ($wires_on == 1)
{
   # Number of wires in one corner
$NumberCornerUWires = int( $TPCWirePlaneLength/($UWirePitch/$CosUAngle) );

$NumberCornerVWires = int( $TPCWirePlaneLength/($VWirePitch/$CosVAngle) );

   # Total number of wires touching one vertical (longer) side
   # Note that the total number of wires per plane is this + another set of corner wires
$NumberSideUWires = int( $TPCWirePlaneHeight/($UWirePitch/$SinUAngle) );

$NumberSideVWires = int( $TPCWirePlaneHeight/($VWirePitch/$SinVAngle) );

   # Number of wires per side that aren't cut off by the corner
$NumberCommonUWires = $NumberSideUWires - $NumberCornerUWires;

$NumberCommonVWires = $NumberSideVWires - $NumberCornerVWires;

   # number of wires on the vertical plane
$NumberVerticalWires = int( $TPCWirePlaneLength/$XWirePitch );
}

# These XML comments throughout make the GDML file easier to navigate
print TPC <<EOF;

<!--+++++++++++++++++++ U Wire Solids ++++++++++++++++++++++-->

EOF

# The corner wires for the U plane
if ($wires_on==1) 
{
    for ($i = 0; $i < $NumberCornerUWires; ++$i)
    {
  # Subtraction to avoid corners of wires overlapping the TPCPlane sides,
  # equal to 0.5*TCPWireThickness*($TanUAngle+1/$TanUAngle),
  # allowing for 30deg<UAngle

   print TPC <<EOF;
    <tube name="TPCWireU$i"
      rmax="0.5*$TPCWireThickness"
      z="$UWirePitch*($TanUAngle+1/$TanUAngle)*($i+1)-0.01732"
      deltaphi="360"
      aunit="deg"
      lunit="cm"/>
EOF

    }
  # Next, the wire used many times in the middle of the U plane.
  # Subtraction again to avoid wire corners overlapping, equal to 
  # 0.5*TCPWireThickness*2/$TanVAngle, allowing for 30deg<VAngle

   print TPC <<EOF;
    <tube name="TPCWireUCommon"
      rmax="0.5*$TPCWireThickness"
      z="$TPCWirePlaneLength/$SinUAngle-0.02598"
      deltaphi="360"
      aunit="deg"
      lunit="cm"/>
EOF

} else { 
#inform the gdml user why no wires show up when -w=0 is used

print TPC <<EOF;


   <!-- The command -w=0 has been used when running generate_lbne_gdml-NEW.pl -->

        <!-- This GDML version has no wires and uses much less memory -->

EOF

}

print TPC <<EOF;









<!--+++++++++++++++++++ V Wire Solids ++++++++++++++++++++++-->


EOF

# The corner wires for the V plane
if ($wires_on==1) 
{
    for ($i = 0; $i < $NumberCornerVWires; ++$i)
    {
    # Same subtraction to avoid corners of wires overlapping 
    # the TPCPlane sides

	print TPC <<EOF;

    <tube name="TPCWireV$i"
      rmax="0.5*$TPCWireThickness"
      z="$VWirePitch*($TanVAngle+1/$TanVAngle)*($i+1)-0.01732"
      deltaphi="360"
      aunit="deg"
      lunit="cm"/>

EOF

    }

    # The wire used many times in the middle of the V plane
    # Same subtraction as U common

   print TPC <<EOF;
    <tube name="TPCWireVCommon"
      rmax="0.5*$TPCWireThickness"
      z="$TPCWirePlaneLength/$SinVAngle-0.02598"
      deltaphi="360"
      aunit="deg"
      lunit="cm"/>
EOF

} else { 
#inform the gdml user why no wires show up when -w=0 is used

print TPC <<EOF;


                   <!-- no wires in this GDML -->

EOF

}



# Begin structure and create the cathode and vertical wire logical volume
print TPC <<EOF;
</solids>
<structure>
    <volume name="volCathode">
      <materialref ref="STEEL_STAINLESS_Fe7Cr2Ni" />
      <solidref ref="Cathode" />
    </volume>
    <volume name="volTPCActive">
      <materialref ref="LAr"/>
      <solidref ref="TPCActive"/>
    </volume>









<!--+++++++++++++++++ Wire Logical Volumes ++++++++++++++++++++-->

EOF


if ($wires_on==1) 
{ 
  print TPC <<EOF;
    <volume name="volTPCWireVert">
      <materialref ref="STEEL_STAINLESS_Fe7Cr2Ni" />
      <solidref ref="TPCWireVert" />
    </volume>
EOF

  # Corner U wires logical volumes
  for ($i = 0; $i < $NumberCornerUWires; ++$i)
  {
  print TPC <<EOF;
    <volume name="volTPCWireU$i">
      <materialref ref="STEEL_STAINLESS_Fe7Cr2Ni" />
      <solidref ref="TPCWireU$i" />
    </volume>
EOF

  }

  # Common U wire logical volume, referenced many times
  print TPC <<EOF;
    <volume name="volTPCWireUCommon">
      <materialref ref="STEEL_STAINLESS_Fe7Cr2Ni" />
      <solidref ref="TPCWireUCommon" />
    </volume>
EOF

  # Corner V wires logical volumes
  for ($i = 0; $i < $NumberCornerVWires; ++$i)
  {
  print TPC <<EOF;
    <volume name="volTPCWireV$i">
      <materialref ref="STEEL_STAINLESS_Fe7Cr2Ni" />
      <solidref ref="TPCWireV$i" />
    </volume>
EOF

  }

  # Common V wire logical volume, referenced many times
  print TPC <<EOF;
    <volume name="volTPCWireVCommon">
      <materialref ref="STEEL_STAINLESS_Fe7Cr2Ni" />
      <solidref ref="TPCWireVCommon" />
    </volume>
EOF

} else { 
#inform the gdml user why no wires show up when -w=0 is used

print TPC <<EOF;


   <!-- The command -w=0 has been used when running generate_lbne_gdml-NEW.pl -->

        <!-- This GDML version has no wires and uses much less memory -->

EOF

}




#+++++++++++++++++++++++++ Position physical wires ++++++++++++++++++++++++++

#            ++++++++++++++++++++++  U Plane  +++++++++++++++++++++++

# Create U plane logical volume
print TPC <<EOF;







<!--+++++++++++++++++++++ U Plane ++++++++++++++++++++++++-->


    <volume name="volTPCPlaneU">
      <materialref ref="LAr"/>
      <solidref ref="TPCPlane"/>
EOF

if ($wires_on==0)
{
print TPC <<EOF;

           <!-- no wires -->

EOF

} else {

# Starting with the bottom left corner wires:
   # x=0 to center the wires in the plane
   # y positioning: (-0.5*$TPCWirePlaneHeight) starts the incremental increase
        # from the bottom of the plane, and trigonometry gives the increment
   # z positioning: Looking at the plane from the positive x direction,
        # (0.5*$TPCWirePlaneLength) starts the incremental increase from
        # the lower left corner.
   # rotation: same as common wire in code below

for ($i = 0; $i < $NumberCornerUWires; ++$i)
{
my $ypos = (-0.5*$TPCWirePlaneHeight)+0.5*($i+1)*$UWire_yint;
my $zpos = (0.5*$TPCWirePlaneLength)-0.5*($i+1)*$UWire_zint;

my $diff=(0.5*$TPCWirePlaneLength)-0.5*($NumberCornerUWires)*$UWire_zint;
my $zpos=$zpos-$diff;

print TPC <<EOF;
      <physvol>
        <volumeref ref="volTPCWireU$i"/>
        <position name="posTPCWireU$i" unit="cm" x="0" y="$ypos " z="$zpos"/>
        <rotationref ref="rPlusUAngleAboutX"/>
      </physvol>
EOF

}


# Moving upwards to the common wires:
   # x and z are zero to center the wires along a vertical axis
   # y positioning: The trick is positioning the lowest common wire so that the pitch
        # is consistent, then the increment is double the increment of
        # the corner wires since there is no z incriment.
   # rotation: wires in  \\\\  direction, so +90deg to bring them to vertical and 
        # +UAngle counterclockwise to arrive at proper orientation
# Note that the counter maintains wire number (in pos. name) counting bottom to top

for ($i = $NumberCornerUWires; $i < $NumberSideUWires; ++$i)
{
my $ypos = (-0.5*$TPCWirePlaneHeight)+0.5*($NumberCornerUWires)*$UWire_yint+($i+1-$NumberCornerUWires)*$UWire_yint;

print TPC <<EOF;
      <physvol>
        <volumeref ref="volTPCWireUCommon"/>
        <position name="posTPCWireU$i" unit="cm" x="0" y="$ypos " z="0"/>
        <rotationref ref="rPlusUAngleAboutX"/>
      </physvol>
EOF

}


# Finally moving to the corner wires on the top right:
   # x=0 to center the wires in the plane
   # y positioning: plug wire number into same equation
   # z positioning: start at z=0 and go negatively at the same z increment
   # rotation: same as common wire in code above
# note that the counter maintains wire number shown in the position name

for ($i = $NumberSideUWires; $i < $NumberSideUWires+$NumberCornerUWires-1; ++$i)
{
   # Make a counter to recall the right logical volume reference:
   # We want the last U wire in this loop (the highest wire) to be the 
   # first wire in the logical volume loop for U wires. 

$j = $NumberSideUWires+$NumberCornerUWires - $i - 2;

   # Note that since we are referencing the same logical volumes/same solids for
   # the top wires as well as the bottom, the pattern of "stacking" wire on top of wire
   # with an incremental separation is likely to cause the top corner wires to be a 
   # a little shorter than they can be, but never any longer. There is no immediately
   # elegant way to fix this, but at 5mm pitch and around 45deg wire orientation, the
   # wires can be at most 1cm shorter than possible which is negligible until the top
   # 20 wires or so where 1cm is >5% of their length. This also means that there
   # could be one more space for a wire left over, but that is highly unlikely.

my $ypos = (-0.5*$TPCWirePlaneHeight)+0.5*($NumberCornerUWires)*$UWire_yint+($NumberCommonUWires)*$UWire_yint+0.5*($i+1-$NumberSideUWires)*$UWire_yint;
my $zpos = -0.5*($i+1-$NumberSideUWires)*$UWire_zint;

print TPC <<EOF;
      <physvol>
        <volumeref ref="volTPCWireU$j"/>
        <position name="posTPCWireU$i" unit="cm" x="0" y="$ypos " z="$zpos"/>
        <rotationref ref="rPlusUAngleAboutX"/>
      </physvol>
EOF

}

} #ends else


#            ++++++++++++++++++++++  V Plane  +++++++++++++++++++++++

# End U plane and create V plane logical volume
print TPC <<EOF;
    </volume>







<!--+++++++++++++++++++++ V Plane ++++++++++++++++++++++++-->


    <volume name="volTPCPlaneV">
      <materialref ref="LAr"/>
      <solidref ref="TPCPlane"/>
EOF

if ($wires_on==0)
{
print TPC <<EOF;

           <!-- no wires -->

EOF

} else {


# Starting with the bottom right corner wires:
   # x=0 to center the wires in the plane
   # y positioning: (-0.5*$TPCWirePlaneHeight) starts the incremental increase
        # from the bottom of the plane, and trigonometry gives the increment
   # z positioning: Looking at the plane from the positive x direction,
        # (-0.5*$TPCWirePlaneLength) starts the incremental increase from 
        # the lower right corner.
   # rotation: same as common wire in code below

for ($i = 0; $i < $NumberCornerVWires; ++$i)
{
my $ypos = (-0.5*$TPCWirePlaneHeight)+0.5*($i+1)*$VWire_yint;
my $zpos = (-0.5*$TPCWirePlaneLength)+0.5*($i+1)*$VWire_zint;

my $diff=(-0.5*$TPCWirePlaneLength)+0.5*($NumberCornerVWires)*$VWire_zint;
my $zpos=$zpos-$diff;

print TPC <<EOF;
      <physvol>
        <volumeref ref="volTPCWireV$i"/>
        <position name="posTPCWireV$i" unit="cm" x="0" y="$ypos " z="$zpos"/>
        <rotationref ref="rPlusVAngleAboutX"/>
      </physvol>
EOF

}


# Moving upwards to the common wires:
   # x and z are zero to center the wires along a vertical axis
   # y positioning: Plug wire number into the same corner ypos equation
   # rotation: wires in  ////  direction, so +90deg to bring them to vertical and 
        # --VAngle counterclockwise to arrive at proper orientation
# Note that the counter maintains wire number in the position name

for ($i = $NumberCornerVWires; $i < $NumberSideVWires; ++$i)
{
my $ypos = (-0.5*$TPCWirePlaneHeight)-0.5*($NumberCornerVWires)*$VWire_yint+($i+1)*$VWire_yint;

print TPC <<EOF;
      <physvol>
        <volumeref ref="volTPCWireVCommon"/>
        <position name="posTPCWireV$i" unit="cm" x="0" y="$ypos " z="0"/>
        <rotationref ref="rPlusVAngleAboutX"/>
      </physvol>
EOF

}


# Finally moving to the corner wires on the top right:
   # x=0 to center the wires in the plane
   # y positioning: plug wire number into same equation
   # z positioning: start at z=0 and go positively at the same z increment
   # rotation: same as common wire in code above
# note that the counter maintains wire number shown in the position name

for ($i = $NumberSideVWires; $i < $NumberSideVWires+$NumberCornerVWires-1; ++$i)
{
   # Make a counter to recall the right logical volume reference where the last
   # wire in this loop is the smallest, first wire in the logical volume loop, just as in U

$j = $NumberSideVWires+$NumberCornerVWires - $i - 2;

   # Note that since we are referencing the same logical volumes/same solids for
   # the top wires as well as the bottom, the pattern of "stacking" wire on top of wire
   # with an incremental separation is likely to cause the top corner wires to be a 
   # a little shorter than they can be, but never any longer. Just as in U

my $ypos = (-0.5*$TPCWirePlaneHeight)+0.5*($NumberCornerVWires)*$VWire_yint+($NumberCommonVWires)*$VWire_yint+0.5*($i+1-$NumberSideVWires)*$VWire_yint;
my $zpos = 0.5*($i+1-$NumberSideVWires)*$VWire_zint;

print TPC <<EOF;
      <physvol>
        <volumeref ref="volTPCWireV$j"/>
        <position name="posTPCWireV$i" unit="cm" x="0" y="$ypos " z="$zpos"/>
        <rotationref ref="rPlusVAngleAboutX"/>
      </physvol>
EOF
}

} #ends else



#            ++++++++++++++++++++++  X Plane  +++++++++++++++++++++++

# End V plane and create X plane logical volume
print TPC <<EOF;
    </volume>






<!--+++++++++++++++++++++ X Plane ++++++++++++++++++++++++-->


    <volume name="volTPCPlaneX">
      <materialref ref="LAr"/>
      <solidref ref="TPCPlane"/>
EOF

if ($wires_on==0)
{
print TPC <<EOF;

           <!-- no wires -->

EOF

} else {

# This is the simplest plane, one loop creates all of the wires
   # x and y position at zero to center the wires
   # z position: moving from front of detector to back, in the positive z direction,
        # starting at (-0.5*$TPCWirePlaneLength), the right side looking from 
        # the +x direction

for ($i=0; $i<$NumberVerticalWires; ++$i)
{
my $zpos = (-0.5*$TPCWirePlaneLength)+$XWirePitch*($i+1);

print TPC <<EOF;
      <physvol>
        <volumeref ref="volTPCWireVert"/>
        <position name="posTPCWireX$i" unit="cm" x="0" y="0 " z="$zpos"/>
        <rotationref ref="rPlus90AboutX" />
      </physvol>
EOF

}

} #ends else

print TPC <<EOF;
    </volume>
EOF

#+++++++++++++++++++++ Position physical wires Above +++++++++++++++++++++

#wrap up the TPC file
print TPC <<EOF;
    <volume name="volTPC">
      <materialref ref="LAr" />
      <solidref ref="TPC" />
     <physvol>
       <volumeref ref="volCathode" />
       <position name="posCathode" unit="cm" x="$TPCWidth/2-$CPAThickness/2" y="0" z="0" />
     </physvol>
     <physvol>
       <volumeref ref="volTPCPlaneU" />
       <position name="posTPCPlaneU" unit="cm" x="(-$TPCWidth/2)+3*$APAWirePlaneSpacing" y="0" z="0" />
     </physvol>
     <physvol>
       <volumeref ref="volTPCPlaneV" />
       <position name="posTPCPlaneV" unit="cm" x="(-$TPCWidth/2)+2*$APAWirePlaneSpacing" y="0" z="0" />
     </physvol>
     <physvol>
       <volumeref ref="volTPCPlaneX" />
       <position name="posTPCPlaneX" unit="cm" x="(-$TPCWidth/2)+$APAWirePlaneSpacing" y="0" z="0" />
     </physvol>
     <physvol>
       <volumeref ref="volTPCActive"/>
       <positionref ref="posTPCActive"/>
     </physvol>
    </volume>
</structure>
</gdml>
EOF

    close(GDML);

} #end of sub gen_TPC





#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++++ gen_Cryostat +++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_Cryostat()
{

# Create the cryostat fragment file name,
# add file to list of output GDML fragments,
# and open it
    $CRYO = "lbne_10kT_Cryostat" . $suffix . ".gdml";
    push (@gdmlFiles, $CRYO);
    $CRYO = ">" . $CRYO;
    open(CRYO) or die("Could not open file $CRYO for writing");


# The standard XML prefix and starting the gdml
    print CRYO <<EOF;
<?xml version='1.0'?>
<gdml>
EOF


# All the cryostat solids.
print CRYO <<EOF;
<solids>
    <box name="Cryostat" lunit="cm" 
      x="$CryostatWidth" 
      y="$CryostatHeight" 
      z="$CryostatLength"/>
    <box name="ArgonInterior" lunit="cm" 
      x="$ArgonWidth"
      y="$ArgonHeight+$HeightGaseousAr"
      z="$ArgonLength"/>
    <box name="LiquidArgon" lunit="cm" 
      x="$ArgonWidth"
      y="$ArgonHeight"
      z="$ArgonLength"/>
    <subtraction name="SteelShell">
      <first ref="Cryostat"/>
      <second ref="ArgonInterior"/>
    </subtraction>
    <box name="LightPaddle" lunit="cm"
      x="$LightPaddleWidth"
      y="$LightPaddleHeight"
      z="$LightPaddleLength"/>
</solids>
EOF

# Cryostat structure
print CRYO <<EOF;
<structure>
    <volume name="volSteelShell">
      <materialref ref="STEEL_STAINLESS_Fe7Cr2Ni" />
      <solidref ref="SteelShell" />
    </volume>
    <volume name="volLiquidArgon">
      <materialref ref="LAr"/>
      <solidref ref="LiquidArgon"/>
    </volume>
    <volume name="volLightPaddle">
      <materialref ref="Acrylic"/>
      <solidref ref="LightPaddle"/>
    </volume>
    <volume name="volCryostat">
      <materialref ref="ArGas" />
      <solidref ref="Cryostat" />
      <physvol>
        <volumeref ref="volLiquidArgon"/>
        <position name="posLiquidArgon" unit="cm" x="0" y="-$HeightGaseousAr/2" z="0"/>
      </physvol>
      <physvol>
        <volumeref ref="volSteelShell"/>
        <position name="posSteelShell" unit="cm" x="0" y="0" z="0"/>
      </physvol>
EOF

# nested for loops to place the non-rotated AND rotated volTPC
   # x loop rotation: There are six drift volumes. Looking into the 
   # detector from incident direction, and counting from right (-x) to
   # left (+x), odd number volumes need to be rotated 180deg about Y in
   # order for the cathode to be on the right of the APA.

if ($tpc_on==1) {


  for($k=0 ; $k<$nAPALong ; $k++)
  {

	$TPC_Z = -$ArgonLength/2 + $UpstreamLArPadding + $APALength/2 + $k*($APALength+$APALongGap); 
		
		for($i=0 ; $i<$nAPAWide+1 ; $i++)
		{

		       $CAT_X       = -$FiducialWidth/2 + $CPAThickness/2 + $i*($APAWidth);
                       $APACenter   = -$FiducialWidth/2 + $CPAThickness/2 + ($i+0.5)*($APAWidth);
                       $TPC_X_rot   = $APACenter-($TPCWidth+$APAFrameThickness)/2;
                       $TPC_X       = $APACenter+($TPCWidth+$APAFrameThickness)/2;
                       #for the rotation, remember the wires are on the negative side of volTPC


				for($j=0 ; $j<$nAPAHigh ; $j++)
				{    # Make sure this is done in a way that the coordinate system is RH 		

				    $TPC_Y = $FiducialHeight/2 -$HeightGaseousAr/2 - $APAHeight/2 - $j*($APAHeight+$APAVerticalGap); 
print CRYO <<EOF;

     <physvol>
       <volumeref ref="volCathode" />
       <position name="posCathode\-$i\-$j\-$k" unit="cm" x="$CAT_X" y="$TPC_Y" z="$TPC_Z" />
     </physvol>

EOF

# This if statement is to stop placement of this last set of TPCs/LightPaddles when placing
# the last set of CPAs that do not have any drift volume beyond them in +x

 if ($i<$nAPAWide){
print CRYO <<EOF;

      <physvol>
        <volumeref ref="volTPC"/>
        <position name="posTPCrot\-$i\-$j\-$k" unit="cm" x="$TPC_X_rot" y="$TPC_Y" z="$TPC_Z"/>
	<rotationref ref="rPlus180AboutY"/>
      </physvol>
      <physvol>
        <volumeref ref="volTPC"/>
        <position name="posTPC\-$i\-$j\-$k" unit="cm" x="$TPC_X" y="$TPC_Y" z="$TPC_Z"/>
	<rotationref ref="rIdentity"/>
      </physvol>

EOF

#if($i=0 and $j<2 and $k=0){
 for ($paddle = 0; $paddle<$nLightPaddlesPerAPA; $paddle++){

# All Light Paddle centers will have the same
#	X coordinate as the center of the current APA
#	Z coordinate as the current TPC pair
# The Y coordinate must be looped over:

#the multiplication by j here is a temporary dirty trick to get around some strange behavior

   $Paddle_Y = $TPC_Y - $APAHeight/2 + (1-$j)*$FrameToPaddleSpace + $j*$LightPaddleHeight + $PaddleYInterval*$paddle; 

print CRYO <<EOF;

     <physvol>
       <volumeref ref="volLightPaddle"/>
       <position name="posPaddle\-$paddle\-TPC\-$i\-$j\-$k" unit="cm" x="$APACenter" y="$Paddle_Y" z="$TPC_Z"/>
       <rotationref ref="rIdentity"/>
     </physvol>

EOF

				}#end Paddle for-loop
#}
		    }#end if

				} #high
		} #wide
  } #long

}# end if tpc

print CRYO <<EOF;
    </volume>
</structure>
</gdml>
EOF

close(CRYO);
}



#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++ gen_Enclosure +++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_Enclosure()
{

# Create the detector enclosure fragment file name,
# add file to list of output GDML fragments,
# and open it
    $ENCL = "lbne_10kT_DetEnclosure" . $suffix . ".gdml";
    push (@gdmlFiles, $ENCL);
    $ENCL = ">" . $ENCL;
    open(ENCL) or die("Could not open file $ENCL for writing");


# The standard XML prefix and starting the gdml
    print ENCL <<EOF;
<?xml version='1.0'?>
<gdml>
EOF


# All the detector enclosure solids.
print ENCL <<EOF;
<solids>

    <box name="FoamPadBlock" lunit="cm"
      x="$CryostatWidth+2*$FoamPadding"
      y="$CryostatHeight"
      z="$CryostatLength+2*$FoamPadding" />

    <subtraction name="FoamPadding">
      <first ref="FoamPadBlock"/>
      <second ref="Cryostat"/>
      <position name="posCryoInFoam" x="0" y="0" z="0"/>
    </subtraction>

    <box name="ConcreteWall" lunit="cm"
      x="$ArToAr - 2*$FoamPadding"
      y="$CryostatHeight+$ConcretePadding"
      z="$CryostatLength+2*$TotalPadding"/>

    <box name="DetEnclosure" lunit="cm" 
      x="$DetEncWidth"
      y="$DetEncHeight"
      z="$DetEncLength"/>

</solids>
EOF

#remember...
#$DetEncWidth	      =	2*$CryostatWidth+2*$TotalPadding+$ArToAr;
#$DetEncHeight	      =	$CryostatHeight+$ConcretePadding;
#$DetEncLength        = $CryostatLength+2*$TotalPadding;

# Detector enclosure structure
    print ENCL <<EOF;
<structure>
    <volume name="volFoamPadding">
      <materialref ref="fibrous_glass"/>
      <solidref ref="FoamPadding"/>
    </volume>

    <volume name="volConcreteWall">
      <materialref ref="Concrete"/>
      <solidref ref="ConcreteWall"/>
    </volume>

    <volume name="volDetEnclosure">
      <materialref ref="Concrete"/>
      <solidref ref="DetEnclosure"/>
EOF

	# option for one cryostat
if($nCryos==1){
    print ENCL <<EOF;
    <physvol>
        <volumeref ref="volFoamPadding"/>
        <position name="posNegFoamCryo" unit="cm" x="-$CryostatWidth/2-$ArToAr/2" y="$ConcretePadding/2" z="0"/>
    </physvol>
    <physvol>
      <volumeref ref="volCryostat"/>
      <position name="posNegCryo" unit="cm" x="-$CryostatWidth/2-$ArToAr/2" y="$ConcretePadding/2" z="0" />
    </physvol>
EOF
		} elsif ($nCryos==2) {
    print ENCL <<EOF;
    <physvol>
      <volumeref ref="volFoamPadding"/>
      <position name="posNegFoamCryo" unit="cm" x="-$CryostatWidth/2-$ArToAr/2" y="$ConcretePadding/2" z="0"/>
    </physvol>
    <physvol>
      <volumeref ref="volCryostat"/>
      <position name="posNegCryo" unit="cm" x="-$CryostatWidth/2-$ArToAr/2" y="$ConcretePadding/2" z="0" />
    </physvol>
    <physvol>
      <volumeref ref="volConcreteWall"/>
      <position name="posConcreteWall" unit="cm" x="0" y="0" z="0"/>
    </physvol>
    <physvol>
      <volumeref ref="volFoamPadding"/>
      <position name="posPosFoamCryo" unit="cm" x="$CryostatWidth/2+$ArToAr/2" y="$ConcretePadding/2" z="0"/>
    </physvol>
    <physvol>
      <volumeref ref="volCryostat"/>
      <position name="posPosCryo" unit="cm" x="$CryostatWidth/2+$ArToAr/2" y="$ConcretePadding/2" z="0" />
    </physvol>
EOF
		}


    print ENCL <<EOF;
    </volume>
EOF

print ENCL <<EOF;
</structure>
</gdml>
EOF

close(ENCL);
}

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++ gen_ServiceBuilding +++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_ServiceBuilding()
{

# Create the service building above the detector enclosure
# add file to list of output GDML fragments,
# and open it
    $SRVBUILD = "lbne_10kT_DetServiceBuilding" . $suffix . ".gdml";
    push (@gdmlFiles, $SRVBUILD);
    $SRVBUILD = ">" . $SRVBUILD;
    open(SRVBUILD) or die("Could not open file $SRVBUILD for writing");


# The standard XML prefix and starting the gdml
    print SRVBUILD <<EOF;
<?xml version='1.0'?>
<gdml>
EOF


# All the detector enclosure solids.
print SRVBUILD <<EOF;
<solids>

    <box name="LowBaySolid" lunit="cm"
      x="$LowBayInsideWidth + 2*$LowBaySideWallThickness"
      y="$LowBayInsideHeight + $LowBayCeilingThickness"
      z="$LowBayInsideLength + $LowBayBackWallThickness"/>

    <box name="LowBayInsideSpace" lunit="cm"
      x="$LowBayInsideWidth"
      y="$LowBayInsideHeight"
      z="$LowBayInsideLength" />

    <subtraction name="LowBayWalls">
      <first ref="LowBaySolid"/>
      <second ref="LowBayInsideSpace"/>
      <position name="posLowBayInsideSpace" unit="cm" x="0" y="-$LowBayCeilingThickness/2-0.001" z="$LowBayBackWallThickness/2+0.01"/>
    </subtraction>

    <box name="HighBaySolid" lunit="cm"
      x="$HighBayInsideWidth + 2*$HighBaySideWallThickness"
      y="$HighBayInsideHeight + $HighBayCeilingThickness"
      z="$HighBayInsideLength + $HighBayBackWallThickness + $HighBayFrontWallThickness"/>

    <box name="HighBayInsideSpace" lunit="cm"
      x="$HighBayInsideWidth"
      y="$HighBayInsideHeight"
      z="$HighBayInsideLength" />

    <box name="HighBayRearOpening" lunit="cm" x="$DetEncWidth"
      y="$LowBayInsideHeight" z="$HighBayBackWallThickness+0.01"/>

    <subtraction name="HighBayTemp">
      <first ref="HighBaySolid"/>
      <second ref="HighBayInsideSpace"/>
      <position name="posHighBayInsideSpace" unit="cm" x="0" y="-$HighBayCeilingThickness/2-0.001" z="$HighBayBackWallThickness/4"/>
    </subtraction>

    <subtraction name="HighBayWalls">
      <first ref="HighBayTemp"/>
      <second ref="HighBayRearOpening"/>
      <position name="posHighBayRearOpening" unit="cm" x="0" y="-(($HighBayInsideHeight+$HighBayCeilingThickness)/2 - $LowBayInsideHeight/2)" z="-($HighBayBackWallThickness/4+$HighBayInsideLength/2)"/>
    </subtraction>

    <box name="FrontGabion" lunit="cm"
      x="$HighBayInsideWidth + 2*$HighBaySideWallThickness + 2*$GabionThickness"
      y="$HighBayInsideHeight + $HighBayCeilingThickness +$HighBayTopRockThickness"
      z="$GabionThickness"/>

    <box name="SideGabion" lunit="cm"
      x="$GabionThickness"
      y="$HighBayInsideHeight + $HighBayCeilingThickness +$HighBayTopRockThickness"
      z="$HighBayInsideLength + $HighBayFrontWallThickness +$HighBayBackWallThickness"/>

    <box name="RearGabion" lunit="cm"
      x="$GabionThickness"
      y="$LowBayInsideHeight + $LowBayCeilingThickness +$LowBayTopRockThickness"
      z="$LowBayInsideLength +$LowBayBackWallThickness"/>

    <box name="HighBayTopRock" lunit="cm"
      x="$HighBayInsideWidth+2*$HighBaySideWallThickness"
      y="$HighBayTopRockThickness"
      z="$HighBayInsideLength + $HighBayFrontWallThickness + $HighBayBackWallThickness"/>

    <box name="LowBayTopRock" lunit="cm"
      x="$LowBayInsideWidth+2*$LowBaySideWallThickness"
      y="$LowBayTopRockThickness"
      z="$LowBayInsideLength + $LowBayBackWallThickness"/>

    <arb8 name="SlopeRockFill" v1x="0" v1y="0" v2x="$SlopeRockFillLength" v2y="0" v3x="0.01" v3y="$SlopeRockFillHeight" v4x="0" v4y="$SlopeRockFillHeight" v5x="0" v5y="0" v6x="$SlopeRockFillLength" v6y="0" v7x="0.01" v7y="$SlopeRockFillHeight" v8x="0" v8y="$SlopeRockFillHeight" dz="$LowBayInsideWidth/2+$LowBaySideWallThickness + $GabionThickness" lunit="cm"/>

    <arb8 name="HillSide" v1x="0" v1y="0" v2x="$HillSideHeight" v2y="0" v3x="0.001" v3y="$HillSideLength" v4x="0" v4y="$HillSideLength" v5x="0" v5y="0" v6x="$HillSideHeight" v6y="0" v7x="0.001" v7y="$HillSideLength" v8x="0" v8y="$HillSideLength" dz="$HillSideWidth/2" lunit="cm"/>

    <arb8 name="HillSideMiddle" v1x="0" v1y="0" v2x="$HillSideMiddleHeight" v2y="0" v3x="0.001" v3y="$HillSideMiddleLength" v4x="0" v4y="$HillSideMiddleLength" v5x="0" v5y="0" v6x="$HillSideMiddleHeight" v6y="0" v7x="0.001" v7y="$HillSideMiddleLength" v8x="0" v8y="$HillSideMiddleLength" dz="$HillSideMiddleWidth/2" lunit="cm"/>

    <box name="HillSideBox" lunit="cm"
      x="$HillSideBoxWidth"
      y="$HillSideBoxHeight"
      z="$HillSideBoxLength"/>

</solids>
EOF


# ServiceBuilding structure
    print SRVBUILD <<EOF;
<structure>
   <volume name="volHighBay">
      <materialref ref="Concrete"/>
      <solidref ref="HighBayWalls"/>
   </volume>
   <volume name="volLowBay">
      <materialref ref="Concrete"/>
      <solidref ref="LowBayWalls"/>
   </volume>
   <volume name="volFrontGabion">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="FrontGabion"/>
   </volume>
   <volume name="volSideGabion1">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="SideGabion"/>
   </volume>
   <volume name="volSideGabion2">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="SideGabion"/>
   </volume>

   <volume name="volRearGabion1">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="RearGabion"/>
   </volume>
   <volume name="volRearGabion2">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="RearGabion"/>
   </volume>

   <volume name="volHighBayTopRock">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="HighBayTopRock"/>
   </volume>

   <volume name="volLowBayTopRock">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="LowBayTopRock"/>
   </volume>

   <volume name="volSlopeRockFill">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="SlopeRockFill"/>
   </volume>

   <volume name="volHillSideBox">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="HillSideBox"/>
   </volume>

   <volume name="volHillSideMiddle">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="HillSideMiddle"/>
   </volume>

   <volume name="volHillSide1">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="HillSide"/>
   </volume>

   <volume name="volHillSide2">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="HillSide"/>
   </volume>

EOF

print SRVBUILD <<EOF;
</structure>
</gdml>
EOF

close(SRVBUILD);
}

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++ gen_HillSide +++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_HillSide()
{
# Create the WORLD fragment file name,
# add file to list of output GDML fragments,
# and open it
    $HILLSIDE = "lbne_10kT_HillSide" . $suffix . ".gdml";
    push (@gdmlFiles, $HILLSIDE);
    $HILLSIDE = ">" . $HILLSIDE;
    open(HILLSIDE) or die("Could not open file $HILLSIDE for writing");


# The standard XML prefix and starting the gdml
    print HILLSIDE <<EOF;
<?xml version='1.0'?>
<gdml>
EOF


# All the World solids.
print HILLSIDE <<EOF;
<solids>

    <box name="Ground" lunit="cm"
      x="$DetEncWidth+2*$RockThickness"
      y="$DetEncHeight+$RockThickness"
      z="$DetEncLength+2*$RockThickness"/>
    <box name="GroundRock" lunit="cm"
      x="$DetEncWidth+2*$RockThickness"
      y="$DetEncHeight+$RockThickness"
      z="$DetEncLength+2*$RockThickness"/>

    <box name="DetectorCavern" lunit="cm"
      x="$DetEncWidth"
      y="$DetEncHeight+0.001" 
      z="$DetEncLength"/>

    <box name="HighBayFloorSpace" lunit="cm"
      x="$HighBayInsideWidth + $HighBaySideWallThickness"
      y="$ConcretePadding"
      z="$HighBayInsideLength - $HighBayOverlap"/>

    <subtraction name="GroundRockTemp">
      <first ref="GroundRock"/>
      <second ref="HighBayFloorSpace"/>
      <position name="posCavernInGround" unit="cm" x="0" y="$RockThickness/2" z="0"/>
    </subtraction>

    <subtraction name="GroundRockWithCavern">
      <first ref="GroundRockTemp"/>
      <second ref="DetectorCavern"/>
      <position name="posCavernInGround" unit="cm" x="0" y="$RockThickness/2" z="0"/>
    </subtraction>

</solids>
EOF

# World structure
print HILLSIDE <<EOF;
<structure>

    <volume name="volGroundRockWithCavern">
      <materialref ref="DUSEL_Rock"/>
      <solidref ref="GroundRockWithCavern"/>
    </volume>

    <volume name="volGround">
      <materialref ref="Air"/>
      <solidref ref="Ground"/>
      <physvol>
        <volumeref ref="volGroundRockWithCavern"/>
        <position name="posGroundRockWithCavern" unit="cm" x="0" y="-($RockThickness+$DetEncHeight)/2 + $DetEncHeight/2" z="0"/>
      </physvol>
      <physvol>
        <volumeref ref="volFrontGabion"/>
        <position name="posFrontGabion" unit="cm" x="0" y="$DetEncHeight/2 +($HighBayInsideHeight+$HighBayCeilingThickness)/2 + $HighBayTopRockThickness/2" z="($DetEncLength/2+($HighBayInsideLength+$HighBayFrontWallThickness) -$HighBayOverlap) +$GabionThickness/2"/>
      </physvol>
      <physvol>
        <volumeref ref="volSideGabion1"/>
        <position name="posSideGabion" unit="cm" x="-($DetEncWidth+$ServiceBuildingExtraWidth+2*$HighBaySideWallThickness+$GabionThickness)/2" y="$DetEncHeight/2 +($HighBayInsideHeight+$HighBayCeilingThickness)/2 + $HighBayTopRockThickness/2" z="($DetEncLength/2+($HighBayInsideLength+$HighBayBackWallThickness+$HighBayFrontWallThickness)/2 -$HighBayBackWallThickness - $HighBayOverlap)"/>
      </physvol>
      <physvol>
        <volumeref ref="volSideGabion2"/>
        <position name="posSideGabion" unit="cm" x="($DetEncWidth+$ServiceBuildingExtraWidth+2*$HighBaySideWallThickness+$GabionThickness)/2" y="$DetEncHeight/2 +($HighBayInsideHeight+$HighBayCeilingThickness)/2 + $HighBayTopRockThickness/2" z="($DetEncLength/2+($HighBayInsideLength+$HighBayBackWallThickness+$HighBayFrontWallThickness)/2 -$HighBayBackWallThickness - $HighBayOverlap)"/>
      </physvol>
      <physvol>
        <volumeref ref="volRearGabion1"/>
        <position name="posRearGabion" unit="cm" x="-($DetEncWidth+$ServiceBuildingExtraWidth+2*$LowBaySideWallThickness+$GabionThickness)/2" y="$DetEncHeight/2 +($LowBayInsideHeight+$LowBayCeilingThickness)/2 + $LowBayTopRockThickness/2" z="-(($LowBayInsideLength+$LowBayBackWallThickness)/2 + $HighBayOverlap+$HighBayBackWallThickness - $DetEncLength/2)"/>
      </physvol>
      <physvol>
        <volumeref ref="volRearGabion2"/>
        <position name="posRearGabion" unit="cm" x="($DetEncWidth+$ServiceBuildingExtraWidth+2*$LowBaySideWallThickness+$GabionThickness)/2" y="$DetEncHeight/2 +($LowBayInsideHeight+$LowBayCeilingThickness)/2 + $LowBayTopRockThickness/2" z="-(($LowBayInsideLength+$LowBayBackWallThickness)/2 + $HighBayOverlap+$HighBayBackWallThickness - $DetEncLength/2)"/>
      </physvol>
      <physvol>
        <volumeref ref="volHighBayTopRock"/>
        <position name="posHighBayTopRock" unit="cm" x="0" y="$DetEncHeight/2 + $HighBayInsideHeight + $HighBayCeilingThickness + $HighBayTopRockThickness/2" z="($DetEncLength/2+($HighBayInsideLength+$HighBayBackWallThickness+$HighBayFrontWallThickness)/2 -$HighBayBackWallThickness - $HighBayOverlap)"/>
      </physvol>
      <physvol>
        <volumeref ref="volLowBayTopRock"/>
        <position name="posLowBayTopRock" unit="cm" x="0" y="$DetEncHeight/2 + $LowBayInsideHeight + $LowBayCeilingThickness + $LowBayTopRockThickness/2" z="-(($LowBayInsideLength+$LowBayBackWallThickness)/2 + $HighBayOverlap+$HighBayBackWallThickness - $DetEncLength/2)"/>
      </physvol>
      <physvol>
        <volumeref ref="volSlopeRockFill"/>
        <position name="posSlopeRockFill" unit="cm" x="0" y="$DetEncHeight/2+($LowBayInsideHeight+$LowBayCeilingThickness) +$SlopeRockFillHeight/3" z="-( $HighBayOverlap+$HighBayBackWallThickness - $DetEncLength/2)"/>
	<rotationref ref="rMinus90AboutY"/>
      </physvol>
      <physvol>
        <volumeref ref="volHillSideBox"/>
        <position name="posHillSideBox" unit="cm" x="0" y="$DetEncHeight/2 + $HillSideBoxHeight/2" z="-($DetEncLength/2 +$HillSideBoxLength/2 + ($LowBayInsideLength +$LowBayBackWallThickness + $HighBayBackWallThickness + $HighBayOverlap - $DetEncLength ) )"/>
      </physvol>

      <physvol>
        <volumeref ref="volHillSideMiddle"/>
        <position name="posHillSideMiddle" unit="cm" x="0" y="$DetEncHeight/2 + $HillSideBoxHeight" z="-($DetEncLength/2 +$HillSideBoxLength + ($LowBayInsideLength +$LowBayBackWallThickness + $HighBayBackWallThickness + $HighBayOverlap - $DetEncLength ) )"/>
	<rotationref ref="rMinus90AboutYMinus90AboutX"/>
      </physvol>

      <physvol>
        <volumeref ref="volHillSide1"/>
        <position name="posHillSide" unit="cm" x="-($DetEncWidth+$ServiceBuildingExtraWidth+2*$LowBaySideWallThickness+2*$GabionThickness+$HillSideWidth)/2" y="$DetEncHeight/2" z="-$RockThickness-$DetEncLength/2"/>
	<rotationref ref="rMinus90AboutYMinus90AboutX"/>
      </physvol>
      <physvol>
        <volumeref ref="volHillSide2"/>
        <position name="posHillSide" unit="cm" x="($DetEncWidth+$ServiceBuildingExtraWidth+2*$LowBaySideWallThickness+2*$GabionThickness+$HillSideWidth)/2" y="$DetEncHeight/2" z="-$RockThickness-$DetEncLength/2"/>
	<rotationref ref="rMinus90AboutYMinus90AboutX"/>
      </physvol>
    </volume>
</structure>
</gdml>
EOF

# make_gdml.pl will take care of <setup/>

close(HILLSIDE);
}


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++ gen_World +++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_World()
{

# Create the WORLD fragment file name,
# add file to list of output GDML fragments,
# and open it
    $WORLD = "lbne_10kT_World" . $suffix . ".gdml";
    push (@gdmlFiles, $WORLD);
    $WORLD = ">" . $WORLD;
    open(WORLD) or die("Could not open file $WORLD for writing");


# The standard XML prefix and starting the gdml
    print WORLD <<EOF;
<?xml version='1.0'?>
<gdml>
EOF


# All the World solids.
print WORLD <<EOF;
<solids>
    <box name="World" lunit="cm" 
      x="$DetEncWidth+3*$RockThickness" 
      y="$DetEncHeight+3*$RockThickness" 
      z="$DetEncLength+3*$RockThickness"/>
</solids>
EOF

# World structure
print WORLD <<EOF;
<structure>
    <volume name="volWorld" >
      <materialref ref="Air"/>
      <solidref ref="World"/>
<!--   <physvol>
     <volumeref ref="volHighBay"/>
     <position name="posHighBay" unit="cm" x="0" y="$DetEncHeight/2+($HighBayInsideHeight+$HighBayCeilingThickness)/2" z="($DetEncLength/2+($HighBayInsideLength+$HighBayBackWallThickness+$HighBayFrontWallThickness)/2 -$HighBayBackWallThickness - $HighBayOverlap)"/>
   </physvol>
   <physvol>
     <volumeref ref="volLowBay"/>
     <position name="posLowBay" unit="cm" x="0" y="$DetEncHeight/2+($LowBayInsideHeight+$LowBayCeilingThickness)/2" z="-(($LowBayInsideLength+$LowBayBackWallThickness)/2 + $HighBayOverlap+$HighBayBackWallThickness - $DetEncLength/2)"/>
   </physvol>
      <physvol>
        <volumeref ref="volGround"/>
        <position name="posGround" unit="cm" x="0" y="0" z="0"/>
      </physvol> -->
      <physvol>
        <volumeref ref="volDetEnclosure"/>
        <position name="posDetEnclosure" unit="cm" x="0" y="0" z="0"/>
      </physvol>
    </volume>
</structure>
</gdml>
EOF

# make_gdml.pl will take care of <setup/>

close(WORLD);
}

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++ write_fragments ++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub write_fragments()
{
   # This subroutine creates an XML file that summarizes the the subfiles output
   # by the other sub routines - it is the input file for make_gdml.pl which will
   # give the final desired GDML file. Specify its name with the output option.
   # (you can change the name when running make_gdml)

   # This code is taken straigh from the similar MicroBooNE generate script, Thank you.

    if ( ! defined $output )
    {
	$output = "-"; # write to STDOUT 
    }

    # Set up the output file.
    $OUTPUT = ">" . $output;
    open(OUTPUT) or die("Could not open file $OUTPUT");

    print OUTPUT <<EOF;
<?xml version='1.0'?>

<!-- Input to Geometry/gdml/make_gdml.pl; define the GDML fragments
     that will be zipped together to create a detector description. 
     -->

<config>

   <constantfiles>

      <!-- These files contain GDML <constant></constant>
           blocks. They are read in separately, so they can be
           interpreted into the remaining GDML. See make_gdml.pl for
           more information. 
	   -->
	   
EOF

    foreach $filename (@defFiles)
    {
	print OUTPUT <<EOF;
      <filename> $filename </filename>
EOF
    }

    print OUTPUT <<EOF;

   </constantfiles>

   <gdmlfiles>

      <!-- The GDML file fragments to be zipped together. -->

EOF

    foreach $filename (@gdmlFiles)
    {
	print OUTPUT <<EOF;
      <filename> $filename </filename>
EOF
    }

    print OUTPUT <<EOF;

   </gdmlfiles>

</config>
EOF

    close(OUTPUT);
}
