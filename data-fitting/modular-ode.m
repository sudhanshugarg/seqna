function [t,y] = processData(fname,col1,col2,left,right,options, yl, yn)
   [t,y] = readlhcfile(fname , col1, col2, left/60, right/60);
   t = t - t(1);

   %Can be optional
   if(options(1))
       lastNvalues = options(1);
   else
       lastNvalues = 10;
   endif

   %2 signifies whether to
   %read the value from yl or not.
   if(options(2))
        ait = min(yl);
    else
        ait = min(y);
   endif

   %3 signifies whether to
   %read the value from yn or not.
   if(options(3))
      aft = mean(yn(end-lastNvalues:end));
   else
      aft = mean(y(end-lastNvalues:end));
   endif
%  disp(aft);

   %4 signifies whether to normalize
   %according to conc.
   if(options(4))
       conc = options(4);
       y = mynormalize(y,ait,aft,conc);
   endif
   
   %5 signifies whether to smooth or not.
   if(options(5))
        y = smooth(y,21);
   endif

   %6 signifies whether to normalize
   %according to mintime.
   mintime = Inf;
   if(options(6))
       mintime = options(6);
   endif
   [t,y] = mycutrange(t,y,mintime);

   %7 signifies whether to zero or not.
   if(options(7))
        y = zeros(1,length(y));
   endif

endfunction


function s = solveode2(fn, init)

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%%%%%%Initial Concentrations - usually
    %%%%%% passed in through init.
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   init(1) = 0; %A1
   init(2) = 0; %TB1
   init(4) = 0; %A2
   init(6) = 0; %A3
   init(12) = 0; %TA1
   init(14) = 0; %TA
   init(19) = 87.46; %B1
   init(21) = 92.95; %B2
   init(23) = 86.38; %B3
   init(30) = 0; %TB
   init(35) = 94; %RC

   conc = min([init(19), init(21)]);
   conc = min([conc, init(23)]);

   mintime = Inf;

   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
   %DEFAULT OPTIONS
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
   %taking last values to mean
   options(1) = 10;
   %normalize from another curve
   options(2) = 0;
   options(3) = 1;
   %concentration
   options(4) = conc;
   %smooth
   options(5) = 1;
   %mintime
   options(6) = mintime;
   %to zero or not.
   options(7) = 0;

   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
   %READ DATA IN
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
   %cuvette 3
   options(1) = 100;
   options(3) = 1;
   options(5) = 0;
   %[t1x3,y1x3] = processData('sampledata.csv' , 3, 4, 760, 7830,options,0,0);
   [t1x2,y1x2] = readlhcfile('sampledata.csv' , 1, 2, 7900/60, 9900/60);
   [t0x2,y0x2] = processData('sampledata.csv' , 1, 2, 767, 7836,options,0,y1x2);

   tcalc = t0x2;
   ycalc = y0x2;

   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   %Calculate Fit.
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

   k = zeros(9,1);
   kmethod = zeros(9,1);
   %kmethod(i) = 1 => using iterative, else default using doubling.
   %currently all unimolecular reaction use iterative methods
   %rate of hairpin + initiator strand
   k(1) = 2.3928e-4; %DONE

   % rate of displacement of initiator (unimolecular)
   k(2) = 5.05e-16; %DONE
   kmethod(2) = 1;

   % rate of toehold exchange - reversibly
   k(3) = 1.7e-4; %DONE

   % rate of B2 displacing RC irreversibly.
   k(4) = 9.22e+2;

   % rate of two hairpins opening up
   k(5) = 5.05e-15; %DONE, leak rate
   %k(5) = 5.52e-07; %DONE, leak rate

   % rate of just hairpin B2 opening up RC
   %k(6) = 5.76e-07; %DONE
   k(6) = 5.05e-14; %DONE

   % rate of two single strands hybridizing.
   k(7) = 4e-3; %kta
   %kta = 7e-2; 

   % rate of duplex dehybridizing
   k(8) = 5e-6; %krta
   kmethod(8) = 1;

   % rate of RC duplex dehybridizing
   k(9) = 3.9672e-3; %RC opening up spontaneously.
   kmethod(9) = 1;

   %33 is the TET curve, and 1000 is max number of iterations.
   %k is the initial set of values being passed.
   curve = 33;
   %64 chosen since assuming that the upper limit of any rate constant is
   %2^64 * 1e-16 ~ 2k /nMs, which is quite fast.
   [kfit] = fit_rate_constants(tcalc,ycalc,fn,k,kmethod,init,curve,64);

   g = @(x,t) fn(x,t,kfit);
   try
   fitcalc = lsode(g, init, tcalc);
   catch
       printf("Error occured is lsode, continuing.\n");
       myflush();
   end_try_catch

   fit = fitcalc(:,curve);

   %{
       init(2) = 95.47;
   %}

   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
   %PLOT DATA
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
   l = length(init);
   i = 1;
   plotColor = rand(20,3);
   h = figure(1);

   ops = {
            'tcalc', 'ycalc', 'B123 0x Cuv 3 Apr 18'
            'tcalc', 'fit', 'fit for curve'
        };

        hold on;
        len = size(ops,1);
        for i=1:len
            plot(eval([ops{i,1}]), eval([ops{i,2}]), 'color', plotColor(i,:));
            leg{i} = ops{i,3};
        endfor
        legend(leg);
        hold off;
        
       i++;
   s = 1;
        ax = gca();
        set(ax, 'fontsize', 15);
        filename = round(time() - 1399320000);
        filename = [num2str(filename) '.jpg']
        print (filename, '-djpg');
endfunction

function s = compare1_2Orders(k1,k2,conc,t)
B = conc*exp(-k1*(t-t(1)));
C = 1./(1/conc + k2*(t-t(1)));
plot(t,B,'r', t, C, 'g');
legend('First Order', 'Second Order');
endfunction

function idx = find_last_less_than(arr, value)
    i=1;
    while((i < length(arr)) && (arr(i) < value))
        i++;
    endwhile
    idx = i;
endfunction

function val = mymin (varargin)
  val = min ([varargin{:}]);
endfunction

function [smallest,avg] = myminmean(y,last)
   smallest = min(y);
   avg = mean(y(end-last:end));
endfunction

function normalized = mynormalize(y,ai,af,conc)
   normalized = (y-ai)*conc/(af-ai);
endfunction

function [tm,val] = mycutrange(t,y,mintime)
   i = find_last_less_than(t, mintime);
   tm = t(1:i);
   val = y(1:i);
endfunction
