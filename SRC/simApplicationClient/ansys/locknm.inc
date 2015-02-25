* include,locknm                                                         jas

c *** copyright(c) 2009 SAS IP, Inc.  All rights reserved.
c *** ansys, inc.

c   **********  define symbolic names for various locks  **********

c  *** IMPORTANT NOTE:  THE MAXIMUM LOCK NUMBER IS DEFINED IN PPCOM.INC
c                       WE CANNOT EXCEED THAT LOCK NUMBER HERE UNLESS WE
c                       INCREASE THE VALUE IN PPCOM.INC (CURRENTLY 150)

c        *****  ansys locks  *****
      integer
     x LOCKNULL      , LOCKTRACK     ,
     x LOCKMM        , LOCKDG        , LOCKIR        , LOCKTM        ,
     x LOCKLOCAL1    , LOCKDP        , LOCKGP        , LOCKYT        ,
     x LOCKSK        , LOCKHHT       , LOCKEG        , LOCKLV        ,
     x LOCKSE        , LOCKRF        , LOCKRQ        , LOCKCV        ,
     x LOCKNL        , LOCKAT        , LOCKSD        , LOCKSV        ,
     x LOCKDB        , LOCKOT        , LOCKER        , LOCKVIRT      ,
     x LOCKMT        , LOCKRT        , LOCKIO        , LOCKCUT       ,
     x LOCKEADD      , LOCKVL        , LOCKVM        , LOCKSN        ,
     x LOCKSXNL      , LOCKKP        , LOCKES        , LOCKGR        ,
     x LOCKRS        , LOCKQRDAMP    , LOCKSZ        , LOCKLAST      ,
     x LOCKNLIST     , LOCKDAMPFLG   , LOCKNXREF     , LOCKESURF     ,
     x LOCKMSG       , LOCKRDAMP11   , LOCKEADJ      , LOCKZROT      ,
     x LOCKWFILE     , LOCKTIMCHK    , LOCKRNGWRN    , LOCKPGRPH     ,
     x LOCKPROP      , LOCKX11       , LOCKDBG       , LOCKESHAP     ,
     x LOCKDREC      , LOCKECWRN     , LOCKRZN       , LOCKSUB       ,
     x LOCKSEFILE    , LOCKDSUB      , LOCKUI        , LOCKEOT       ,
     x LOCKTZ        , LOCKPORT      , LOCKINISTR    , LOCKCCI       ,
     x LOCKEDG       , LOCKUPF       , LOCKSLV       , LOCKEL89      ,
     x LOCKNOROT     , LOCKELMT      , LOCKPLOT      , LOCKPUT       ,
     x LOCKSEL       , LOCKPML       , LOCKMXSVREC   , LOCKSELAY     ,
     x LOCKSLTRK     , LOCKMGRM      , LOCKCONT      , LOCKPXYZ      ,
     x LOCKSOCKET    , LOCKFL116     , LOCKPTRI      , LOCKMEMMGR    ,
     x LOCKPCGIO     , LOCKPCGCNT    , LOCKPCGSTF    , LOCKPCGMAS    ,
     x LOCKPCGDMP    , LOCKPCGBD     , LOCKPCGRHS    , LOCKPCGTIM    ,
     x LOCKPCGVTAV   , LOCKPCGDGFP   , LOCKPCGBKFP   , LOCKPCGEFP    ,
     x LOCKPCGWRN    , LOCKPCGERR    , LOCKCONTACT   , LOCKPCGASF    ,
     x LOCKNCMP      , LOCKLEMCC     , LOCKCADOEMEM  , LOCKWRNCNT    ,
     x LOCKSECSZE    , LOCKCYCLS     , LOCKTIMER     , LOCKCPXRST    ,
     x LOCKTHKWRN    , LOCKLMPWRN    , LOCKEDISTRT   , LOCKSFLEX     ,
     x LOCKSL39      , LOCKACEL59    , LOCKNEGPRS    , LOCKMACFUCH   ,
     x LOCKSTR289    , LOCKSBT290    , LOCKLYRTMP    , LOCKR1281     ,
     x LOCKNEGRAD


      parameter  (
     x LOCKNULL=-1   , LOCKTRACK=-1  ,
     x LOCKMM=1      , LOCKDG=2      , LOCKIR=3      , LOCKTM=4      ,
     x LOCKLOCAL1=5  , LOCKDP=6      , LOCKGP=7      , LOCKYT=8      ,
     x LOCKSK=9      , LOCKHHT=10    , LOCKEG=11     , LOCKLV=12     ,
     x LOCKSE=13     , LOCKRF=14     , LOCKRQ=15     , LOCKCV=16     ,
     x LOCKNL=17     , LOCKAT=18     , LOCKSD=19     , LOCKSV=20     ,
     x LOCKDB=21     , LOCKOT=22     , LOCKER=23     , LOCKVIRT=24   ,
     x LOCKMT=25     , LOCKRT=26     , LOCKIO=27     , LOCKCUT=28    ,
     x LOCKEADD=29   , LOCKVL=30     , LOCKVM=31     , LOCKSN=32     ,
     x LOCKSXNL=33   , LOCKKP=34     , LOCKES=35     , LOCKGR=36     ,
     x LOCKRS=37     , LOCKQRDAMP=38 , LOCKSZ=39     , LOCKLAST=40   ,
     x LOCKNLIST=41  , LOCKDAMPFLG=42, LOCKNXREF=43  , LOCKESURF=44  ,
     x LOCKMSG=45    , LOCKRDAMP11=46, LOCKEADJ=47   , LOCKZROT=48   ,
     x LOCKWFILE=49  , LOCKTIMCHK=50 , LOCKRNGWRN=51 , LOCKPGRPH=52  ,
     x LOCKPROP=53   , LOCKX11=54    , LOCKDBG=55    , LOCKESHAP=56  ,
     x LOCKDREC=57   , LOCKECWRN=58  , LOCKRZN=59    , LOCKSUB=60    ,
     x LOCKSEFILE=61 , LOCKDSUB=62   , LOCKUI=63     , LOCKEOT=64    ,
     x LOCKTZ=65     , LOCKPORT=66   , LOCKINISTR=67 , LOCKCCI=68    ,
     x LOCKEDG=69    , LOCKUPF=70    , LOCKSLV=71    , LOCKEL89=72   ,
     x LOCKNOROT=73  , LOCKELMT=74   , LOCKPLOT=75   , LOCKPUT=76    ,
     x LOCKSEL=77    , LOCKPML=78    , LOCKMXSVREC=79, LOCKSELAY=80  ,
     x LOCKSLTRK=81  , LOCKMGRM=82   , LOCKCONT=83   , LOCKPXYZ=84   ,
     x LOCKSOCKET=85 , LOCKFL116=86  , LOCKPTRI=87   , LOCKMEMMGR=88 ,
     x LOCKPCGIO=89  , LOCKPCGCNT=90 , LOCKPCGSTF=91 , LOCKPCGMAS=92 ,
     x LOCKPCGDMP=93 , LOCKPCGBD=94  , LOCKPCGRHS=95 , LOCKPCGTIM=96 ,
     x LOCKPCGVTAV=97, LOCKPCGDGFP=98, LOCKPCGBKFP=99, LOCKPCGEFP=100,
     x LOCKPCGWRN=101, LOCKPCGERR=102, LOCKCONTACT=103,LOCKPCGASF=104,
     x LOCKNCMP=105  , LOCKLEMCC=106 , LOCKCADOEMEM=107,LOCKWRNCNT=108,
     x LOCKSECSZE=109, LOCKCYCLS=110 , LOCKTIMER=111 , LOCKCPXRST=112,
     x LOCKTHKWRN=113, LOCKLMPWRN=114, LOCKEDISTRT=115,LOCKSFLEX=116 ,
     x LOCKSL39=117  , LOCKACEL59=118, LOCKNEGPRS=119, LOCKMACFUCH=120,
     x LOCKSTR289=121, LOCKSBT290=122, LOCKLYRTMP=123, LOCKR1281=124 ,
     x LOCKNEGRAD=125)

c       *****  flotran locks  *****
      integer    
     x LOCKFD        , LOCKAS        , LOCKSC        , LOCKTU        ,
     x LOCKPK        , LOCKTS        , LOCKWL        , LOCKCF        ,
     x LOCKDI
      parameter  (
     x LOCKFD=12     , LOCKAS=14     , LOCKSC=15     , LOCKTU=16     ,
     x LOCKPK=17     , LOCKTS=18     , LOCKWL=20     , LOCKCF=25     ,
     x LOCKDI=26)



co  LOCKNULL      -1 - dummy lock for analyzer (very heavily used)
co  LOCKTRACK     -1 - dummy lock for Track routines
co  LOCKMM         1 - lock for mass and moment of inertia sum
co  LOCKDG         2 - lock for minimum/maximum diagonal terms
co  LOCKIR         3 - lock for inertia relief sums (rlfelm.F)
co  LOCKTM         4 - lock for cpu time accumulations
co  LOCKLOCAL1     5 - Local lock number 1 (local to a single routine)
co  LOCKDP         6 - lock for kdampd in mcmplx.F and matspd.F
co  LOCKGP         7 - lock for contact surface counter
co  LOCKYT         8 - lock for spline functions from solid modeling (mageva.F)
co  LOCKSK         9 - lock for a couple of unrelated items
co  LOCKHHT       10 - lock for HHT related items
co  LOCKEG        11 - lock for element get
co  LOCKLV        12 - lock for load vector accumulation
co  LOCKFD        12 - lock for flotran Field Updating
co  LOCKSE        13 - lock for strain energy accumulation
co  LOCKRF        14 - lock for reaction force summation
co  LOCKAS        14 - lock for flotran assembly
co  LOCKRQ        15 - lock for rayleigh quotient
co  LOCKSC        15 - lock for flotran Src and SrcDiag Accumulations
co  LOCKCV        16 - lock for convergence data (hystab.F)
co  LOCKTU        16 - lock for flotran Tau and YPlus Accumulation
co  LOCKNL        17 - lock for nonlinear element convergence data (heavily used)
co  LOCKPK        17 - lock for flotran pk Accumulations
co  LOCKAT        18 - lock for auto load data
co  LOCKTS        18 - lock for flotran TempStore
co  LOCKSD        19 - lock for seld file operations
co  LOCKSV        20 - lock for esav file operations (heavily used)
co  LOCKWL        20 - lock for flotran Ceff, Veff at Wall
co  LOCKDB        21 - lock for all data base accesses (very heavily used)
co  LOCKOT        22 - lock for all write statements (very heavily used)
co  LOCKER        23 - lock for error handling
co  LOCKVIRT      24 - lock for low-level database routines (no longer used)
co  LOCKMT        25 - lock for emat file operations
co  LOCKCF        25 - lock for flotran kCeff,kVeff
co  LOCKRT        26 - lock for erot file operations
co  LOCKDI        26 - lock for flotran DeltaInv
co  LOCKIO        27 - lock for buffered I/O routines (bioget.F and bioput.F)
co  LOCKCUT       28 - lock for actCUTelem,CUTelem,actCUTmaterial
co  LOCKEADD      29 - lock for SmAddMatrix for element assembly
co  LOCKVM        30 - lock for vmxxxx database routines 
co  LOCKVL        31 - lock for data base status updates 
co  LOCKSN        32 - lock for max. total strain (heavily used)
co  LOCKSXNL      33 - lock for Cadoe sxnl_xxxx routines
co  LOCKKP        34 - lock for /nopr,/gopr in subroutine echusr
co  LOCKES        35 - lock for element security
co  LOCKGR        36 - lock for graphics output (heavily used)
co  LOCKRS        37 - lock for result file operations
co  LOCKQRDAMP    38 - lock for QR damped common block key (EmatPrep.F)
co  LOCKSZ        39 - lock for size object
co  LOCKLAST      40 - lock for first/last/DofBits tables (sfelem.F)
co  LOCKNLIST     41 - lock for node list object
co  LOCKDAMPFLG   42 - lock for dampFlg common value (sfelem.F/EmatPrep.F)
co  LOCKNXREF     43 - lock for node/element xref object
co  LOCKESURF     44 - lock for element surface object (GESurfCreate,Delete)
co  LOCKMSG       45 - lock for output in lngerr
co  LOCKRDAMP11   46 - lock for rdamp11 parameters (matadj.F)
co  LOCKEADJ      47 - lock for element adjacency table
co  LOCKZROT      48 - lock for counter in d3zrot
co  LOCKWFILE     49 - lock for an ASCII data file (bfipin for example)
co  LOCKTIMCHK    50 - lock for timchk calls
co  LOCKRNGWRN    51 - lock for material range checking warning flag (warnMpEval.F)
co  LOCKPGRPH     52 - lock for P graphics object
co  LOCKPROP      53 - lock for material property buffer + material range checking
co  LOCKX11       54 - lock for status widget in X11 interactive
co  LOCKDBG       55 - lock for debug key (heavily used)
co  LOCKESHAP     56 - lock for element shape checking (elmgck.F)
co  LOCKDREC      57 - lock for D rectangles for edge element, 117, mpg
co  LOCKECWRN     58 - lock for element shape warning/error counters
co  LOCKRZN       59 - lock for rezoning related variables
co  LOCKSUB       60 - lock for Substructure file read/write
co  LOCKSEFILE    61 - lock for Substructure SEFILE read/write
co  LOCKDSUB      62 - lock for Substructure DSUB file read/write
co  LOCKUI        63 - lock for user interface common variables (elcdff.F)
co  LOCKEOT       64 - lock for a couple of unrelated items
co  LOCKTZ        65 - lock for Trefftz element, mpg, dv-13799
co  LOCKPORT      66 - lock for WaveGuide ports (heavily used)
co  LOCKINISTR    67 - lock for Initial Stress file (dmpIniStress)
co  LOCKCCI       68 - lock for Couple Code Interface
co  LOCKEDG       69 - lock for edge elements
co  LOCKUPF       70 - lock for user programmable features
co  LOCKSLV       71 - lock for user solvemethod, mpg dv-527
co  LOCKEL89      72 - lock for entire VISCO89 element routine (el89.F)
co  LOCKNOROT     73 - lock for node_norot common block value (sfelem.F)
co  LOCKELMT      74 - lock for some legacy variables from frontal solver code (sfelem.F)
co  LOCKPLOT      75 - lock for plotting displays
co  LOCKPUT       76 - lock for puts (ndpall.F)
co  LOCKSEL       77 - lock for selects (ndsel1.F)
co  LOCKPML       78 - lock for PML common block variables (hPMLmx.F)
co  LOCKMXSVREC   79 - lock for maximum ESAV file record length (elmatrecord.F)
co  LOCKSELAY     80 - lock for superelement layer warning common variable (layr8i.F)
co  LOCKSLTRK     81 - lock for nonlinear diagnostics track memory
co  LOCKMGRM      82 - lock for magnetic rom, mpg, dv-1109
co  LOCKCONT      83 - lock for contact force sum
co  LOCKPXYZ      84 - lock for rotating primary variables PTAB_X,PTAB_Y,PTAB_Z to local cs
co  LOCKSOCKET    85 - lock for Socket connection MF solver
co  LOCKFL116     86 - lock for some common block variables in el116.F
co  LOCKPTRI      87 - lock for common block variable ptri (poissonN.F)
co  LOCKMEMMGR    88 - lock for the ANSYS memory manager code (cfAnsMemManager.c)
co  LOCKPCGIO     89 - lock for I/O statistics counter variable in PCG code
co  LOCKPCGCNT    90 - lock for global counter variables updated in PCG code
co  LOCKPCGSTF    91 - lock for assembled stiffness matrix in PCG code
co  LOCKPCGMAS    92 - lock for assembled mass matrix in PCG code
co  LOCKPCGDMP    93 - lock for assembled damping matrix in PCG code
co  LOCKPCGBD     94 - lock for BlockDiag structure in PCG code
co  LOCKPCGRHS    95 - lock for assembled rhs vector in PCG code
co  LOCKPCGTIM    96 - lock for global timing variables in PCG code
co  LOCKPCGVTAV   97 - lock for primal preconditioner VTAV structure in PCG code
co  LOCKPCGDGFP   98 - lock for element diagonal file I/O in PCG code
co  LOCKPCGBKFP   99 - lock for block diagonal file I/O in PCG code
co  LOCKPCGEFP   100 - lock for dual preconditioner loweigen file I/O in PCG code
co  LOCKPCGWRN   101 - lock for global warning variable in PCG code
co  LOCKPCGERR   102 - lock for error code return value and related PCG code
co  LOCKCONTACT  103 - lock for all contact data base accesses (heavily used)
co  LOCKPCGASF   104 - lock for out-of-core symbolic assembly logic in PCG code
co  LOCKNCMP     105 - lock for mrNcmp(*) in elrstlen
co  LOCKLEMCC    106 - lock for lemcc (flag for energy/mom. conserving contact)
co  LOCKCADOEMEM 107 - lock for cadoe allocation (cadoemem.cpp)
co  LOCKWRNCNT   108 - lock for element shape checking warnings (qdtrck.F)
co  LOCKSECSZE   109 - lock for section size properties (sectinq.F)
co  LOCKCYCLS    110 - lock for cyclic symmetric load sector (cyc_presFourier.F)
co  LOCKTIMER    111 - lock for Time Manager in CADOE library
co  LOCKCPXRST   112 - lock for complex results post processing code (ampl_vect.F)
co  LOCKTHKWRN   113 - lock for layered-solid thickness warning flag (elsolShlThk.F)
co  LOCKLMPWRN   114 - lock for beam/pipe element lumped mass warning flag
co  LOCKEDISTRT  115 - lock for highly distorted element error message flag
co  LOCKSFLEX    116 - lock for illegal SFLEX options error flag (elPipeSectStif.F)
co  LOCKSL39     117 - lock for COMBIN39 element slope warning flag (el39.F)
co  LOCKACEL59   118 - lock for water table acceleration warning flag
co  LOCKNEGPRS   119 - lock for negative pressure head warning flag
co  LOCKMACFUCH  120 - lock for MacCamy-Fuchs logic warning flag (MacCamy-Fuchs.F)
co  LOCKSTR289   121 - lock for straight/curved PIPE289 warning flag (elinck.F)
co  LOCKSBT290   122 - lock for subtended ELBOW290 warning flag (elinck.F)
co  LOCKLYRTMP   123 - lock for layered element temp warning flag (elcdff.F)
co  LOCKR1281    124 - lock for SHELL181/281 real constant warning flag (elcrcc.F)
co  LOCKNEGRAD   125 - lock for negative radius element error flag (el02/42/56/74/82/88)

