{$DEFINE trial}

{$B-,G+,N-,E-,P+,X+,R-,F-,S+,D-,L-}

uses viewbmp{$IFDEF trial},crt;{$ELSE};{$ENDIF}

type TSpectrumScreen=array[0..6911]of byte;
     TCompressedScreen=array[0..13823]of byte;  { screen-ul comprimat }
                                                { este posibil ca scr compr sa fie }
                                                { mai mare decat cel original }
var fis1:file;
    f:word;attr:byte;
    a1,a2,a3:word;
    ext:string[4];
    vs:virtualscreen;
    vp:palettetype;
    bufzip:record
             len:word;
             scr1:TSpectrumScreen;        { screen-ul necomprimat }
             scr2:TCompressedScreen;      { screen-ul comprimat }
           end;


const inverse:boolean=false;
      bright :byte=0;
      flash  :byte=0;
      ink    :byte=0;
      paper  :byte=7;
      numeimg:string='?';
      compres:integer=0;
      grid:integer=-1;
      attroptions:integer=0;


{$IFDEF trial}{$I trial.inc}{$ENDIF}

procedure AfisHelp;
begin
 WriteLn;
 WriteLn('ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿');
 WriteLn('³        Windows BMP to Spectrum SCR converter             ³');
 WriteLn('³         Autor: Marian Veteanu  (c) VMA soft              ³');
 WriteLn('ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´');
 Writeln('³                                                          ³');
 Writeln('³   Sintax: BMP2SCR <filename> [options]                   ³');
 Writeln('³                                                          ³');
 WriteLn('³   <filename> is a 256x192xB&W .BMP file (6206 bytes)     ³');
 WriteLn('³              or a 256x192x256 .BMP file (50230 bytes)    ³');
 WriteLn('³                                                          ³');
 WriteLn('³   General options:                                       ³');
 WriteLn('³       /I   : inverse pixels from image                   ³');
 WriteLn('³       /F   : use FLASH attr                              ³');
 WriteLn('³       /Gxx : use GRID type xx (bright colors pattern)    ³');
 WriteLn('³       /Cxx : use compression type xx (see documentation) ³');
 WriteLn('³                   xx=0 -> STORE (default)                ³');
 WriteLn('³                   xx=1 -> SIMPLE RLE                     ³');
 WriteLn('³                   xx=2 -> RLE                            ³');
 WriteLn('³                                                          ³');
 WriteLn('³   B/W image options:                                     ³');
 WriteLn('³       /Kxx : use INK color xx                            ³');
 WriteLn('³       /Pxx : use PAPER color xx                          ³');
 WriteLn('³       /B   : use BRIGHT attr                             ³');
 WriteLn('³                                                          ³');
 WriteLn('³   Color image options:                                   ³');
 WriteLn('³       /Axx : screen atributes options                    ³');
 WriteLn('³                   xx=0 -> don''t use BRIGHT               ³');
 WriteLn('³                   xx=1 -> use PAPER to set BRIGH         ³');
 WriteLn('³                   xx=2 -> use INK to set BRIGHT          ³');
 WriteLn('ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ');
 WriteLn;
 Halt;
end;


procedure AflaParametrii;
var f,code,temp:integer;
begin
for f:=1 to paramcount do Begin
 If ((paramstr(f)='/i') or (paramstr(f)='/I')) then inverse:=true;
 If ((paramstr(f)='/b') or (paramstr(f)='/B')) then bright:=1;
 If ((paramstr(f)='/f') or (paramstr(f)='/F')) then flash:=1;
 If copy (paramstr(f),1,1)<>'/' then numeimg:=paramstr(f);
 If (copy (paramstr(f),1,2)='/p') or (copy (paramstr(f),1,2)='/P')
  then begin
         Val(copy(ParamStr(f),3,length(ParamStr(f))-2),temp,code);
         if (code=0)and(temp in [0..7]) then paper:=byte(temp);
       end;
 If (copy (paramstr(f),1,2)='/k') or (copy (paramstr(f),1,2)='/K')
  then begin
         Val(copy(ParamStr(f),3,length(ParamStr(f))-2),temp,code);
         if (code=0)and(temp in [0..7]) then ink:=byte(temp);
       end;
 If (copy (paramstr(f),1,2)='/g') or (copy (paramstr(f),1,2)='/G')
  then begin
         Val(copy(ParamStr(f),3,length(ParamStr(f))-2),temp,code);
         if (code=0)and(temp in [0..5]) then grid:=temp;
       end;
 If (copy (paramstr(f),1,2)='/c') or (copy (paramstr(f),1,2)='/C')
  then begin
         Val(copy(ParamStr(f),3,length(ParamStr(f))-2),temp,code);
         if (code=0)and(temp in [0..2]) then compres:=temp;
       end;
 If (copy (paramstr(f),1,2)='/a') or (copy (paramstr(f),1,2)='/A')
  then begin
         Val(copy(ParamStr(f),3,length(ParamStr(f))-2),temp,code);
         if (code=0)and(temp in [0..2]) then attroptions:=temp;
       end;
End;
end;


{$B-}
Procedure CompresRLE(src:array of byte;var dest:array of byte;var lendest:word);
type Liniar=array[0..65534]of byte;

procedure Compress(a:pointer;len:word;var b:byte;var c:word);
begin
 b:=liniar(a^)[0];c:=0;
 repeat inc(c) until (c>255)or(c>len-1)or(liniar(a^)[c]<>b);
end;

const marker=118;
var   b,f:byte;c:word;
      ps,pd:word;

begin
 ps:=Low(src);pd:=Low(dest);
 while ps<=High(src) do begin
 Compress(@src[ps],High(src)-Low(Src)+1-ps,b,c);
 inc(ps,c);
 if c=256 then c:=0;
 if (c in [1..3]) and (b<>marker)
            then for f:=1 to c do begin
                                   dest[pd]:=b;
                                   inc(pd);
                                  end
            else begin
                  dest[pd+0]:=marker;
                  dest[pd+1]:=b;
                  dest[pd+2]:=c;
                  inc(pd,3);
                 end;
 end;
 lendest:=pd-Low(dest);
end;


Procedure MCGA2SCR(mcga:virtualscreen;pal:palettetype;var scr:TCompressedScreen);
var mcga2:virtualscreen;
    pal2:palettetype;
    x,y,f,lin,col,c:integer;
    paper_,ink_,bright_:byte;

procedure findcolor(sc:virtualscreen;l,c:integer;var paper,ink:byte);
var sir:array[0..63] of byte;nrculinsir:array[0..14] of byte;
    p,i:byte;
    li,ci,f,g:integer;
begin
 for li:=0 to 7 do for ci:=0 to 7 do sir[li*8+ci]:=sc^[l*8+li,c*8+ci];
 for f:=0 to 14 do nrculinsir[f]:=0;
 for f:=0 to 14 do for g:=0 to 63 do if sir[g]=f then inc(nrculinsir[f]);
 p:=7;for f:=0 to 14 do if nrculinsir[f]>nrculinsir[p] then p:=f;
 nrculinsir[p]:=0;
 i:=7;for f:=0 to 14 do if nrculinsir[f]>nrculinsir[i] then i:=f;
 paper:=p;
 ink:=i;
end;

var attr2:array[0..767] of record p,i:byte;end;

begin
 createspectrumpal(pal2);
 setupvirtual(mcga2);
 changescreenpal(mcga,pal,mcga2,pal2);


 for lin:=0 to 23 do for col:=0 to 31 do
   begin
    findcolor(mcga2,lin,col,paper_,ink_);
    attr2[lin*32+col].p:=paper_;
    attr2[lin*32+col].i:=ink_;
   end;


 for lin:=0 to 23 do for col:=0 to 31 do
   begin
    paper_:=attr2[lin*32+col].p;
    ink_:=attr2[lin*32+col].i;
    bright_:=0;
    if paper_>7 then begin dec(paper_,7);if attroptions=1 then bright_:=1;end;
    if ink_>7   then begin dec(ink_,7);  if attroptions=2 then bright_:=1;end;
    bufzip.scr1[6144+lin*32+col]:=ink_+8*paper_+64*bright_+128*flash;
   end;

 for y:=0 to 191 do for x:=0 to 31 do for f:=0 to 7 do
   begin
    if mcga2^[y,x*8+f]=attr2[(y div 8)*32+x].i
        then scr[y*32+x]:=scr[y*32+x] or (256 div (2 shl f));
    if mcga2^[y,x*8+f]=attr2[(y div 8)*32+x].p
        then scr[y*32+x]:=scr[y*32+x] and (255-(256 div (2 shl f)));
   end;

 shutdownvirtual(mcga2);
end;


function bit6(b:byte;state:boolean):byte;
begin
 if state then bit6:=b or 64
          else bit6:=b and 191;
end;

begin
 if ParamCount=0 then AfisHelp else AflaParametrii;

 case BMPType(numeimg) of
  0 : AfisHelp;
  1 : begin
       if not LoadBWBMPFile(numeimg,bufzip.scr2) then AfisHelp;
       attr:=ink+8*paper+64*bright+128*flash;
       for f:=0 to 767 do bufzip.scr1[6144+f]:=attr;
       for f:=0 to 191 do move(bufzip.scr2[(191-f)*32],bufzip.scr1[1792*trunc(f/64)-2016*trunc(f/8)+256*f],32);
       for f:=0 to 6143 do bufzip.scr1[f]:=bufzip.scr1[f] xor 255; {Inverseaza automat imaginea odata}
      end;
  2 : begin
       setupvirtual(vs);
       if not LoadBMPFile(0,0,numeimg,vs,vp) then AfisHelp;
       mcga2scr(vs,vp,bufzip.scr2);
       shutdownvirtual(vs);
       for f:=0 to 191 do move(bufzip.scr2[f*32],bufzip.scr1[1792*trunc(f/64)-2016*trunc(f/8)+256*f],32);
      end;
 end;

 if inverse then for f:=0 to 6143 do bufzip.scr1[f]:=bufzip.scr1[f] xor 255;

 case grid of
  0: for f:=0 to 767 do bufzip.scr1[6144+f]:=bit6(bufzip.scr1[6144+f],false);
  1: for f:=0 to 767 do bufzip.scr1[6144+f]:=bit6(bufzip.scr1[6144+f],true);
  2: for f:=0 to 767 do bufzip.scr1[6144+f]:=bit6(bufzip.scr1[6144+f],odd(f));
  3: for f:=0 to 767 do bufzip.scr1[6144+f]:=bit6(bufzip.scr1[6144+f],odd(f div 32));
  4: for f:=0 to 767 do if odd(f div 32) then bufzip.scr1[6144+f]:=bit6(bufzip.scr1[6144+f],odd(f))
                                         else bufzip.scr1[6144+f]:=bit6(bufzip.scr1[6144+f],not odd(f));
  5: for f:=0 to 767 do if odd(f div 64) then bufzip.scr1[6144+f]:=bit6(bufzip.scr1[6144+f],odd(f div 2))
                                         else bufzip.scr1[6144+f]:=bit6(bufzip.scr1[6144+f],not odd(f div 2));
 end;

 case compres of
  0: begin
      move(bufzip.scr1,bufzip.scr2,6912);
      bufzip.len:=6912;
      ext:='.SCR';
     end;
  1: begin
      a1:=0;a2:=0;a3:=0;
      while a1<=6911 do
      begin
      if bufzip.scr1[a1]<>0
         then begin
                bufzip.scr2[a2]:=bufzip.scr1[a1];
                inc(a1);inc(a2);
              end
         else begin
                inc(a3);
                if a3=255 then begin
                                 bufzip.scr2[a2]:=0;
                                 inc(a2);
                                 bufzip.scr2[a2]:=255;
                                 inc(a2);
                                 a3:=0;
                               end
                          else begin
                                 inc(a1);
                                 if bufzip.scr1[a1]<>0 then begin
                                                       bufzip.scr2[a2]:=0;
                                                       inc(a2);
                                                       bufzip.scr2[a2]:=a3;
                                                       inc(a2);
                                                       a3:=0;
                                                     end;
                               end;
              end;
      end; {while}
     bufzip.len:=a2;
     ext:='.SC1';
     end; {1: begin..end;}
  2: begin
      CompresRLE(bufzip.scr1,bufzip.scr2,bufzip.len);
      ext:='.SC2';
     end; {2: begin..end;}
 end; {case}

 assign(fis1,copy(numeimg,1,Length(numeimg)-4)+ext);rewrite(fis1,1);
 BlockWrite(fis1,bufzip.scr2,bufzip.len);
 close(fis1);
 {$IFDEF trial}ThisProgramIsShareware;{$ENDIF}
end.

