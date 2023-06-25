procedure EfectSonor5;                 { Efecte sonore }
  var f : byte;
    Begin;
       for f:=1 to 200 do begin
           sound(f*Random(50));delay(5);nosound;
                          end;
    end;                               { Efecte sonore }


procedure ThisProgramIsShareware;
var t:integer;
begin
 clrscr;
 writeln('ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿');
 writeln('³              Z80 Tools   -  Shareware version             ³');
 writeln('ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´');
 writeln('³                                                           ³');
 writeln('³     This program is a 30 day Trial version of Z80 Tools   ³');
 writeln('³                                                           ³');
 writeln('³       After the 30 days have expired, if you wish to      ³');
 writeln('³       continue to use Z80 Tools you would need to buy a   ³');
 writeln('³       full product from VMA soft.                         ³');
 writeln('³                                                           ³');
 writeln('³       VMA soft address:                                   ³');
 writeln('³       Email : vma@electra.upit.ro                         ³');
 writeln('³       Web   : http://vmasoft.webjump.com                  ³');
 writeln('³               http://electron.upit.ro/vma                 ³');
 writeln('³       FTP   : ftp://electron.upit.ro/pub/vma              ³');
 writeln('ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ');
 writeln;writeln;
 write('Wait 10 seconds:');EfectSonor5;
 t:=1;
 repeat
  write('.');delay(1000);
  inc(t);
 until t=10;
 writeln;
end;

