type TSpectrumScreen=array[1..6912] of byte;

     Linear=array[0..6936]of byte;

     TapScreenStructure=
        record
           SizeBlock1 : word;
           FlagHeader : byte;
           BlockType  : byte;
           Name       : array[1..10] of char;
           BlockSize  : word;
           Parameter1 : word;
           Parameter2 : word;
           HdrChkSum  : byte;
           SizeBlock2 : word;
           FlagByte   : byte;
           Data       : TSpectrumScreen;
           CheckSum   : byte;
        end;


procedure AfisHelp;
begin
 WriteLn;
 WriteLn('ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿');
 WriteLn('³         Z80 SCR file to Spectrum TAP file           ³');
 WriteLn('³        Autor: Marian Veteanu  (c) VMA soft          ³');
 WriteLn('ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´');
 Writeln('³                                                     ³');
 Writeln('³   Sintax: SCR2TAP <filename>                        ³');
 Writeln('³                                                     ³');
 WriteLn('³   <filename>  is a .SCR file (size 6912 bytes)      ³');
 WriteLn('ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ');
 WriteLn;
 Halt;
end;


procedure LoadScreen(NameCode: string;var buffer:TSpectrumScreen);
var fis:file;
begin
   Assign(fis, NameCode);
   {$I-}Reset(fis, 1);{$I+}
   if ioresult<>0 then AfisHelp;
   if filesize(fis)<>6912 then AfisHelp;
   {$I-}BlockRead(fis,buffer,6912);{$I+}
   if ioresult<>0 then AfisHelp;
   Close(fis);
end;


var tape:TapScreenStructure;
    name:string;f:word;
    fis:file of TapScreenStructure;

begin
 if ParamCount<>1 then AfisHelp;

 tape.SizeBlock1:=19; { lungimea header-ului }
 tape.FlagHeader:=0;
 tape.BlockType:=3;   { date de tip BYTES: }
 tape.Name:='          ';
 tape.BlockSize:=6912;
 tape.Parameter1:=16384;
 tape.Parameter2:=32768;
 tape.HdrChkSum:=0;   { Nu cred ca este calculata corect HdrChkSum ! }
 tape.SizeBlock2:=tape.BlockSize+2;
 tape.FlagByte:=255;
 tape.CheckSum:=0;

 LoadScreen(ParamStr(1),tape.Data);
 name:=copy(ParamStr(1),1,Length(ParamStr(1))-4);
 for f:=1 to length(name) do tape.Name[f]:=name[f];
 for f:=2 to 19 do tape.HdrChkSum:=tape.HdrChkSum xor linear(tape)[f];
 for f:=23 to 6935 do tape.CheckSum:=tape.CheckSum xor linear(tape)[f];

 Assign(fis,name+'.TAP');Rewrite(fis);
 Write(fis,Tape);
 Close(fis);
end.









