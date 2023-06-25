{$G+,X+,R-,F-,S-,D-,L-}
unit ViewBMP;
interface

Type
   PaletteType   = Array [0..255,1..3] of byte;
   VirtualScreen = ^VirtualPage;
   VirtualPage   = Array [0..199,0..319] of byte;
   Linear        = Array [0..63999] of byte;


Function  SetUpVirtual(var VirScr:VirtualScreen):boolean;
Procedure ShutDownVirtual(var VirScr : VirtualScreen);
Procedure ChangeScreenPal(scr1:virtualscreen;pal1:palettetype;scr2:virtualscreen;pal2:palettetype);
Procedure CreateSpectrumPal(var pal:PaletteType);

function BMPType(NameCode:string):integer; {0=nerecunoscut; 1=256x192x2; 2=256x192x8}
function LoadBWBMPFile(NameCode:string;var buffer:array of byte):boolean;
function LoadBMPFile(x,y:LongInt;NameCode:string;scr:virtualscreen;var pal:palettetype):boolean;

type
BitMapFileHeader=record
                   Types:array[1..2] of char;
                   Size:LongInt;
                   Reserved1:word;
                   Reserved2:word;
                   OffBits:LongInt;
                 end;

BitMapInfoHeader=record
                   Size:LongInt;
                   Width:LongInt;
                   Height:LongInt;
                   Planes:word;
                   BitCount:word;
                   Compression:LongInt;
                   SizeImage:LongInt;
                   XPelsPerMeter:LongInt;
                   YPelsPerMeter:LongInt;
                   ClrUsed:LongInt;
                   ClrImportant:LongInt;
                 end;

RGBQuad = record
                   Blue:byte;
                   Green:byte;
                   Red:byte;
                   Reserved:byte;
          end;

BitMapInfo=record
                   H:BitMapInfoHeader;
                   C:array[0..255] of RGBQuad;
           end;

BitMapInfoBW=record
              H:BitMapInfoHeader;
              Dummy:array[1..8]of byte;
             end;


implementation
var
     testH: BitMapFileHeader;
     testI: BitMapInfo;
     testIbw: BitMapInfoBW;
     cRed, cGreen, cBlue: byte;
     cRGB: RGBQuad;
     cReg: byte;
     fBMP: file;
     iBMP, jBMP: LongInt;
     memBMP: byte;
     nrcul:word;
     extra:word;
     readByte: byte;
     LineBuff: array[0..639] of byte;
     Error: Boolean;


Function SetUpVirtual(var VirScr:VirtualScreen):boolean;
Begin
  if maxavail<64000 then begin
                          SetUpVirtual:=false;
                          exit;
                         end;
  GetMem (VirScr,64000);
  SetUpVirtual:=true;
End;


Procedure ChangeScreenPal(scr1:virtualscreen;pal1:palettetype;scr2:virtualscreen;pal2:palettetype);
var dist,savedist,dx,dy,dz:longint;   { Se da o imagine in screen-ul scr1 si }
    f:word;a,b:byte;                  { paleta pal1 si o paleta oarecare in }
    palnoua:array[0..255] of byte;    { pal2. Functia va crea in screen-ul }
function findclosest(pa1,pa2:palettetype;chkcol:byte):byte; { scr2 o imagine }
var i:byte;                           { care 'trebuie sa arate cat mai bine' }
    savecol:byte;                     { in momentul in care i se aplica }
begin                                 { paleta pal2. }
  savedist:=200000;
  for i:=0 to 255 do begin
                dx:=pa2[i,1]-pa1[chkcol,1];
                dy:=pa2[i,2]-pa1[chkcol,2];
                dz:=pa2[i,3]-pa1[chkcol,3];
                dist:=dx*dx+dy*dy+dz*dz;
                if dist < savedist then
                        begin
                          savedist:=dist;
                          savecol:=i;
                        end;
                    end;
  findclosest:=savecol;
end;
begin
 for f:=0 to 255 do palnoua[f]:=findclosest(pal1,pal2,f);
 for f:=0 to 63999 do linear(scr2^)[f]:=palnoua[linear(scr1^)[f]];
end;


Procedure CreateSpectrumPal(var pal:PaletteType);
var f:integer;
begin
 pal[0, 1]:=00; pal[0, 2]:=00; pal[0, 3]:=00;   { Black         }
 pal[1, 1]:=00; pal[1, 2]:=00; pal[1, 3]:=54;   { Blue          }
 pal[2, 1]:=54; pal[2, 2]:=00; pal[2, 3]:=00;   { Red           }
 pal[3, 1]:=54; pal[3, 2]:=00; pal[3, 3]:=54;   { Magenta       }
 pal[4, 1]:=00; pal[4, 2]:=54; pal[4, 3]:=00;   { Green         }
 pal[5, 1]:=00; pal[5, 2]:=54; pal[5, 3]:=54;   { Cyan          }
 pal[6, 1]:=54; pal[6, 2]:=54; pal[6, 3]:=00;   { Yellow        }
 pal[7, 1]:=54; pal[7, 2]:=54; pal[7, 3]:=54;   { White         }
 pal[8, 1]:=00; pal[8, 2]:=00; pal[8, 3]:=63;   { BrightBlue    }
 pal[9, 1]:=63; pal[9, 2]:=00; pal[9, 3]:=00;   { BrightRed     }
 pal[10,1]:=63; pal[10,2]:=00; pal[10,3]:=63;   { BrightMagenta }
 pal[11,1]:=00; pal[11,2]:=63; pal[11,3]:=00;   { BrightGreen   }
 pal[12,1]:=00; pal[12,2]:=63; pal[12,3]:=63;   { BrightCyan    }
 pal[13,1]:=63; pal[13,2]:=63; pal[13,3]:=00;   { BrightYellow  }
 pal[14,1]:=63; pal[14,2]:=63; pal[14,3]:=63;   { BrightWhite   }

 for f:=15 to 255 do begin
                      pal[0,1]:=0;
                      pal[0,2]:=0;
                      pal[0,3]:=0;
                     end;
end;


Procedure ShutDownVirtual(var VirScr:VirtualScreen);
Begin
  FreeMem (VirScr,64000);
End;


function LoadBMPFile;
begin
   Assign(fBMP, NameCode);
   {$I-}Reset(fBMP, 1);{$I+}
   if ioresult<>0 then begin LoadBMPFile:=false;exit;end;

   BlockRead(fBMP, testH, SizeOf(testH));
   if testH.Types<>'BM' then begin Close(fBMP);LoadBMPFile:=false;exit;end;

   BlockRead(fBMP, testI.H, SizeOf(testI.H));
   nrcul:=(testh.offbits-testi.h.size-14) div 4;
   extra:=(testh.size-testI.H.Width*testI.H.Height-testh.offbits) div testI.H.Height;

   if testI.H.BitCount <> 8 then
   begin
     Close(fBMP);
     LoadBMPFile:=false;
     exit;
   end
   else if (testI.H.BitCount = 8) and (testI.H.ClrImportant >= 0) then
      begin
         for iBMP := 0 to nrcul-1 do
             begin
                BlockRead(fBMP, testI.C[iBMP], SizeOf(testI.C[iBMP]));
                testI.C[iBMP].Blue := testI.C[iBMP].Blue div 4;
                testI.C[iBMP].Green := testI.C[iBMP].Green div 4;
                testI.C[iBMP].Red := testI.C[iBMP].Red div 4;
             end;
      end;
   for ibmp:=0 to nrcul-1 do begin
                           pal[ibmp,1]:=testi.c[ibmp].red;
                           pal[ibmp,2]:=testi.c[ibmp].green;
                           pal[ibmp,3]:=testi.c[ibmp].blue;
                         end;

   for iBMP := 0 to testI.H.Height - 1 do
       begin
          BlockRead(fBMP, LineBuff, testI.H.Width+Extra);
          for jBMP := 0 to testI.H.Width - 1 do
              Mem[seg(scr^):jBMP + x +
                         320 * y + 320 * (testI.H.Height - iBMP - 1)] :=
              LineBuff[jBMP];
       end;

   Close(fBMP);
   LoadBMPFile:=true;
end;


function LoadBWBMPFile(NameCode: string ;var buffer : array of byte):boolean;
begin
   Assign(fBMP, NameCode);
   {$I-}Reset(fBMP, 1);{$I+}
   if ioresult<>0 then begin LoadBWBMPFile:=false;exit;end;

   BlockRead(fBMP, testH, SizeOf(testH));
   if testH.Types<>'BM' then begin Close(fBMP);LoadBWBMPFile:=false;exit;end;
   BlockRead(fBMP, testIbw, SizeOf(testIbw));

   if testIbw.H.Width<>256 then begin Close(fBMP);LoadBWBMPFile:=false;exit;end;
   if testIbw.H.Height<>192 then begin Close(fBMP);LoadBWBMPFile:=false;exit;end;
   if testIbw.H.BitCount<>1 then begin Close(fBMP);LoadBWBMPFile:=false;exit;end;

   {$I-}BlockRead(fBMP,buffer,6144);{$I+}
   if ioresult<>0 then begin Close(fBMP);LoadBWBMPFile:=false;exit;end;

   Close(fBMP);
   LoadBWBMPFile:=true;
end;


function BMPType(NameCode:string):integer;
begin
   Assign(fBMP, NameCode);
   {$I-}Reset(fBMP, 1);{$I+}
   if ioresult<>0 then begin BMPType:=0;exit;end;

   BlockRead(fBMP, testH, SizeOf(testH));
   if testH.Types<>'BM' then begin Close(fBMP);BMPType:=0;exit;end;
   BlockRead(fBMP, testI, SizeOf(testI));

   if testI.H.Width<>256  then begin Close(fBMP);BMPType:=0;exit;end;
   if testI.H.Height<>192 then begin Close(fBMP);BMPType:=0;exit;end;

   case testI.H.BitCount of
    1: BMPType:=1;
    8: BMPType:=2;
   else
    BMPType:=0;
   end;

   Close(fBMP);
end;

end.
