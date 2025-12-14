\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{colorful}[2025/12/12 v0.0.1 colorful]
\newcommand{\@baseclass}{ctexbook} % Base class name

% Set the default options
\PassOptionsToClass{a4paper}{\@baseclass}
\PassOptionsToClass{fontsize=10pt}{\@baseclass}
\PassOptionsToClass{parskip=half}{\@baseclass}

% Pass through any other options to the base class
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{\@baseclass}}

%\ProcessKeyvalOptions*
\ProcessOptions\relax % Process the options

\LoadClass{\@baseclass} % Load the base class

\RequirePackage[left=3cm,right=2.5cm,top=2.5cm,bottom=2.5cm]{geometry}
\RequirePackage[svgnames,x11names]{xcolor}
\RequirePackage{amsmath,amssymb,amsfonts}


\RequirePackage{colortbl}
\RequirePackage{fancyhdr}
\RequirePackage[explicit]{titlesec}
\RequirePackage{tikz}
\usetikzlibrary{matrix,fit,calc,shadows,shadows.blur,shapes.symbols,positioning,shapes.geometric,decorations.text}
\RequirePackage[most]{tcolorbox}
\RequirePackage{pifont}
\RequirePackage{multicol}
\RequirePackage{enumerate}
\RequirePackage[shortlabels,inline]{enumitem}
\RequirePackage{eso-pic}
\RequirePackage{tikzpagenodes}
\RequirePackage{nopageno}
%===========================================================
%définition des couleurs
\definecolor{myblueii}{RGB}{63,200,244}
\definecolor{MainRed}{rgb}{.6, .1, .1}
\definecolor{mybluei}{RGB}{0,173,239}
\definecolor{bubblegum}{rgb}{0.99, 0.76, 0.8}
\definecolor{clight2}{RGB}{212, 237, 244}
\definecolor{blueslategray}{cmyk}{0.189, 0.091, 0, 0.439}
\definecolor{greenbrown}{HTML}{555544}
\definecolor{cottonseed}{cmyk}{0, 0.026, 0.047, 0.255}
\definecolor{lightblue}{cmyk}{0.272, 0.068, 0, 0.192}
\definecolor{bluetrout}{cmyk}{0.18, 0.15, 0, 0.608}
\definecolor{brownred}{cmyk}{0, 0.531, 0.557, 0.239}
\definecolor{orangewhite}{cmyk}{0, 0.031, 0.075, 0.004}
\definecolor{avocado}{cmyk}{0.06, 0, 0.55, 0.37}
\definecolor{eggplant}{cmyk}{0.04, 0.24, 0, 0.47}
\definecolor{darkblue}{rgb}{0.12,0.47,0.87}
%===============================================================
%===========================================
\definecolor{shadow@color}{cmyk}{.07,0,0,0.49}
\definecolor{col1}{RGB}{12, 102, 98}
\definecolor{col2}{RGB}{248, 193, 12}
\definecolor{col3}{RGB}{236, 67, 1}
%========================================
%=================================================
\definecolor{gray}{RGB}{236,236,236}
\definecolor{shadow@color}{cmyk}{.07,0,0,0.49}
\definecolor{section@title@color}{cmyk}{1,0.2,0.3,0.1}
\definecolor{subsection@title@color}{cmyk}{0,0.6,0.9,0}
%=================================================
%========================================
%Commands
%newcommand % % % % %
\newcommand\z{\cellcolor{bubblegum}}
\newcommand\y{\cellcolor{clight2}}
% % % % % % % % % % % % % % % % % % % % % % %
\newcommand\highlight[1]{\tikz[overlay, remember picture,baseline=-\the\dimexpr\fontdimen5\textfont2\relax]\node[rectangle,fill=blue!50,rounded corners,fill opacity = 0.2,draw,thick,text opacity =1] {$#1$};}
% % % % % % % % % % % % % % % % % % % % % % %
\newcommand{\tikzmark}[2]{\tikz[overlay,remember picture,baseline=(#1.base)] \node (#1) {#2};}
% % % % % % % % % % % % % % % % % % % % % % %
\newcommand\tikznode[3][]%
{\tikz[remember picture,baseline=(#2.base)]
	\node[minimum size=0pt,inner sep=0pt,#1](#2){#3};%
}
%==============================================
%=============================================
\newlength{\mylen}
\newlength{\hauteur}
\newlength{\malongueur}
\settowidth{\mylen}{方法：}
\setlength{\hauteur}{\baselineskip}
\addtolength{\hauteur}{0.5mm}
\settowidth{\malongueur}{408.52pt}
%\addtolength{\malongueur}{-\mylen}
%===========================================================
\renewcommand{\sfdefault}{lmss}%Latin Modern fonts
%============================================
%==============================================
%fancy chapters
\definecolor{titlepagecolor}{cmyk}{0,0,0,80}
\definecolor{titlepagecolor2}{RGB}{196,175,153}

%\newfont{\chapter@numberfont}{eurb10 scaled 10000}
%\DeclareFixedFont 是 LaTeX 中用于定义固定大小和属性字体的命令。
%与普通的字体命令（如 \bfseries 或 \large）不同，由它定义的字体命令完全忽略周围环境的字体设置。
%无论你在正文、脚注还是标题中使用它，它永远保持定义时的样子。
% 推荐的替换写法:
\DeclareFixedFont{\chapter@numberfont}{U}{eur}{b}{n}{100pt}

\DeclareFixedFont{\bigsf}{T1}{phv}{b}{n}{1cm}
\titlespacing*{\chapter}{0pt}{2cm}{3.5cm}%left,top,bottom

\titleformat{\chapter}[display]
{\bfseries\sffamily\color{Red}}
{%
	\begin{tikzpicture}[remember picture, overlay]
		\filldraw[myblueii](current page.north west)--++(5,0)--++(-80:6)--([shift={(0,-8)}]current page.north west)--cycle;
		\filldraw[subsection@title@color]([shift={(5,0)}]current page.north west)
		--++(-80:6)-|(current page.north east);
		\node[circle,fill=orangewhite,drop shadow={shadow xshift=0pt},minimum size=4cm] (num) at ([shift={(5,-5)}]current page.north west) {\color{darkblue}\chapter@numberfont\thechapter};
		\node[text width=\textwidth-3cm,text height=2cm,text depth=2cm,inner xsep=1cm,font=\Huge\color{white}\bigsf]at ([shift={(10,2)}]num) {#1};
		%====================================
		\path [postaction={decorate,decoration={raise=-1ex,text along path, text align=center,text={|\huge\color{white}|Chapter }}}] (num) circle (3cm);
	\end{tikzpicture}}
{1ex}
{}
[]
%------------------------------------------------------
\titleformat{name=\chapter,numberless}[display]
{\Huge\bfseries\sffamily\color{MainRed}}
{#1}
{1ex}
{\titlerule[2pt]\vspace*{5ex}\huge\normalfont}
%===============================================================
%定制\thesection，只显示节编号
%\renewcommand{\thesection}{\arabic{section}}
\def\sectiontitlefont{\fontfamily{ugq}\selectfont}
%================================================
\newcommand\SecTitle[4]{%
	\hspace*{-1.5cm}\begin{tikzpicture}
		\node[inner xsep=0pt,minimum height=3cm,text width=0.8\textwidth,
		align=left,left color=white,right color=myblueii, signal to=#1,font=\LARGE\bfseries,anchor=#2] (A)
		at (#3,0) {\hspace*{2.25cm}#4};
		%===============================================================
		%==================================================
		\node (numsec) at ($0.5*(A.north west)-0.5*(A.north west)+(1,0)$) {\Huge\textbf{\thesection}};
		\fill[rounded corners=2pt,fill=shadow@color] ($(numsec.north west)+(2pt,-2pt)$) -- ($(numsec.north east)+(1mm,0mm)+(2pt,-2pt)$) -- ($(numsec.south east)+(2pt,-2pt)$) -- ($(numsec.south west)+(-1mm,0)+(2pt,-2pt)$) -- cycle;
		\fill[rounded corners=2pt,fill=section@title@color] (numsec.north west) -- ($(numsec.north east)+(1mm,0mm)$) -- (numsec.south east) -- ($(numsec.south west)+(-1mm,0)$) -- cycle;
		\node[white] at (numsec) {\LARGE\sectiontitlefont\thesection};
	\end{tikzpicture}%
}
%\titleformat{command}[shape]{format}{label}{sep}{before-code}{after-code}
\titleformat{\section}
{\sectiontitlefont\color{section@title@color}}{}{0em}
{\SecTitle{east}{west}{0\paperwidth}{#1}}
%======================================================
%%%%%%%%%%%%%%%%Titre de subsection %%%%%%%%%%%%%%%%%%%%%
%\renewcommand{\thesubsection}{\arabic{subsection}}
\def\subsectiontitlefont{\fontfamily{ugq}\selectfont}
\newcommand\SubsecTitle[4]{%
	\hspace*{-1.5cm}
	\tikz[ overlay]{
		\node (num) at (1.5,0) {\subsectiontitlefont\thesubsection};
		\fill[rounded corners=2pt,fill=shadow@color] ($(num.north west)+(2pt,-2pt)$) -- ($(num.north east)+(1mm,0mm)+(2pt,-2pt)$) -- ($(num.south east)+(2pt,-2pt)$) -- ($(num.south west)+(-1mm,0)+(2pt,-2pt)$) -- cycle;
		\fill[rounded corners=2pt,fill=subsection@title@color] (num.north west) -- ($(num.north east)+(1mm,0mm)$) -- (num.south east) -- ($(num.south west)+(-1mm,0)$) -- cycle;
		\node[white]  at (num) {\subsectiontitlefont\thesubsection};
		\node[color=subsection@title@color, minimum height=.5cm,minimum width=15cm,text width=10cm, align=left,font=\subsectiontitlefont](title) at ([xshift=6cm]num){#4};
	}
}
%\titleformat{command}[shape]{format}{label}{sep}{before-code}{after-code}
\titleformat{\subsection}
{\subsectiontitlefont\color{subsection@title@color}}{}{0em}
{\SubsecTitle{east}{west}{0\paperwidth}{#1}}
%===============================================================
%=============================================
% ===== Environment "definition" ======================
\newtcolorbox[auto counter, number within=chapter]{definition}[1][]{%
	enhanced, breakable, fonttitle=\large\sffamily\bfseries,
	title=定,
	left=\tcboxedtitlewidth,
	boxsep=0pt, top=2mm, right=2mm, bottom=2mm+0.7\baselineskip,
	sharp corners, colback=white, colbacktitle=orange, frame hidden,
	pad before break=-0.7\baselineskip,
	before upper = {\begingroup
			\large\bfseries\textcolor{green!50!black}{义}
			\textcolor{orange}{\thetcbcounter}\quad\endgroup},
	attach boxed title to top left={
		xshift=2pt,
		yshift=-\tcboxedtitleheight+1.5pt-2mm},
	boxed title style={
		empty, left=0mm, right=0mm, top=0mm, bottom=0mm, boxsep=2pt},
	underlay boxed title={
		\path[fill=orange] (title) circle (\tcboxedtitleheight/2);},
	overlay unbroken={
		\coordinate[xshift=\tcboxedtitlewidth/2,
		yshift=-\tcboxedtitleheight-0.7\baselineskip] (nw) at (frame.north west);
		\node (sw) [circle, fill=orange, inner sep=2pt,
		xshift=\tcboxedtitlewidth/2, yshift=\tcboxedtitlewidth/2]
		at (frame.south west) {};
		\draw[orange, line cap=round, line width=2pt,
		dash pattern=on 0pt off 4\pgflinewidth] (nw) -- (sw);},
	overlay first={
		\coordinate[xshift=\tcboxedtitlewidth/2,
		yshift=-\tcboxedtitleheight-0.7\baselineskip] (nw) at (frame.north west);
		\coordinate[xshift=\tcboxedtitlewidth/2,
		yshift=0.7\baselineskip] (sw) at (frame.south west);
		\draw[gray, line cap=round, line width=2pt,
		dash pattern=on 0pt off 4\pgflinewidth] (nw) -- (sw);},
	#1}
%=====================================================
% % % % % %% Example environment
\newcounter{examp}
\setcounter{examp}{0}
\newcommand{\mytitle}[2]%
{%
	\stepcounter{examp}
	\begin{tikzpicture}[font=\sffamily]
		\def\cola{pink!50!purple}
		\def\colb{cyan!80!blue}
		\node[circle,fill=\cola,minimum size=8mm] at (0,0) {};
		\node[fill=\cola,anchor=west,minimum height=8mm,minimum width=2cm] at (0,0) {};
		\node[fill=white,line width=2pt,circle,draw=\cola,minimum size=1cm] at (2.1,0) {\theexamp};
		\node[text=white,anchor=west] at (-0.2,0) {示例};
		\node[fill=\colb,signal,signal to=east,text=white,minimum height=8mm] (arg1) at (4,0) {#1};
		\node[right=2mm of arg1,\colb] (arg2) {#2};
	\end{tikzpicture}
}
%==============================================================
%Exercice résolu
\newtcolorbox{mybox}[2][]{
	enhanced,
	breakable,
	sharp corners,
	rounded corners=northwest,
	colback=white,
	colframe=orange!80!black,
	fonttitle=\bfseries\large\sffamily,
	frame hidden,
	title=#2,
	attach boxed title to top left,
	boxed title size=standard,
	boxed title style={%
		empty,
		rounded corners=north,
		boxrule=0pt,
		bottom=0pt,
	},
	underlay boxed title={%
		\filldraw[rounded corners=\kvtcb@arc, orange!80!black, line width=.5mm]
		(title.south east)--++(93:\tcboxedtitleheight)--++(183:\tcboxedtitlewidth)--++(-87:\tcboxedtitleheight)|-cycle;
		\draw[orange!80!black, line width=.5mm] (title.south)|-(frame.north east);
	},
	#1
}


%===============================================================
%Exercice
%commands
\newcommand{\sect}[1]{\begin{tcolorbox}[colframe=white,top=2pt,bottom=2pt,colback=red!14]
		\centering \textbf{#1}
	\end{tcolorbox}
}
\setlength{\columnsep}{30pt}
\setlength{\columnseprule}{0.2pt}
\setlength{\parindent}{0pt}%
\newtcolorbox[auto counter ]{Exercice}[1][]{
	enhanced,left=0pt,top=8pt,frame hidden,bottom=0pt,before upper={\hspace*{1cm}} ,colback=white,
	interior code app={\node[fill=col3, anchor=west,text width=0.5cm,text badly centered]at ([shift={(0.1,-0.5)}]frame.north west){{\large\color{white} \textbf{\centering\thetcbcounter}}};},nobeforeafter}
%============================================================
%======================================================
%list
%========================================
\newtcbox{\lblbox}{enhanced, drop fuzzy shadow=Gray, tcbox raise base, boxrule=0.6pt, left=1pt, right=2pt, top=0pt, bottom=0pt, colback=cyan!30, colframe=SlateGrey}
%=======================================
\newlist{listexos}{enumerate}{2} % en espérant qu’il n’ait pas plus d’un niveau de sous-questions
\setlist[listexos,1]{label =\lblbox{\arabic*}, font=\itshape\bfseries\large, wide=0pt, leftmargin=*}
%============================================================
%============================================================
%================================================================
\AddToShipoutPictureBG{\ifnum\value{page}=1\begin{tikzpicture}[remember picture,overlay]
		\fill[\BoxColor]([yshift=1cm]current page.south west)rectangle(current page.south east) ;
		%\node[circle,draw=white,line width=2pt,minimum size=0.3cm,fill=\BoxColor,font=\bfseries\color{white}]at([yshift=0.9cm]current page.south){\thepage};
		\fill[subsection@title@color] ([shift={(0,-10)}]current page.north west) rectangle ++(0.8,-8.8);
		\foreach \i [count=\j starting from 1]in {1, 3/2, 11/6, 25/12, 137/60, 49/20, 363/140,
			761/280, 7129/2520, 7381/2520}
		{
			\fill[subsection@title@color] ([yshift={-8.1cm-2*\i*1cm-10cm}]current page.north west)
			rectangle ++(0.8,1/\j);}
		\node[fill=\BoxColor,inner sep=-1pt,rectangle,text width=30pt,
		text height=28cm,align=center,anchor=north east]
		at ($ (current page.north east)+(0,-9) $)
		{};
		\node[anchor=east]at([xshift=-3cm,yshift=.5cm]current page.south){{ \textbf{\color{white}作者：zhao yong }}};
		\node[anchor=west]at([xshift=+3cm,yshift=.5cm]current page.south){{ \textbf{\color{white} \ \ email: lingbao@outlook.com}}};
	\end{tikzpicture}\else
	\begin{tikzpicture}[remember picture,overlay]
		\fill[\BoxColor]([yshift=1cm]current page.south west)rectangle(current page.south east) ;
	\end{tikzpicture}
	\fi }
%===============================================================
%============================================================
\pagestyle{plain}
\newcounter{chapshift}
\addtocounter{chapshift}{-1}
\newcommand\BoxColor{myblueii}
\def\subsectiontitle{}
\renewcommand{\sectionmark}[1]{\markright{\sffamily\normalsize#1}{}}
\renewcommand{\subsectionmark}[1]{\def\subsectiontitle{#1}}
\RequirePackage{etoolbox,fancyhdr}
\pagestyle{fancy}
\renewcommand{\headrule}{{\color{myblueii}%
		\hrule width\headwidth height\headrulewidth depth\headrulewidth}}
\renewcommand{\chaptermark}[1]{\markboth{\sffamily\normalsize\bfseries \ #1}{}}
\renewcommand{\sectionmark}[1]{\markright{\sffamily\normalsize#1}{}}
%======================================================
\newcommand{\Z}{\phantom{Z}} % Filler to define baseline of empty circles
\fancyhf{}
\DeclareRobustCommand{\bul}{%
	\begin{tikzpicture}[baseline={(current bounding box.south)}]
		\fill [\BoxColor,anchor=base,baseline] circle (1mm);
	\end{tikzpicture}
}%

\renewcommand{\chaptermark}[1]{\markboth{第~\chinese{chapter}~章~#1}{}}

\renewcommand{\sectionmark}[1]{\markright{第~\thesection~\bul~#1~节}{}}
\renewcommand\headrulewidth{0pt}
\fancyhead[LO]{\bfseries\color{\BoxColor}\leftmark}
\fancyhead[RE]{\bfseries\color{\BoxColor}\rightmark}
\fancyhead[RO]
{\begin{tikzpicture}[baseline, every node/.style={minimum size=8mm, anchor=base}]
		\path node at (0,0) [shape=circle, fill=cyan!20] (0,0) {\Z}
		node at (1,0) [shape=circle, fill=cyan!40] (0,0) {\Z}
		node at (2,0) [shape=circle, fill=cyan!60,font=\bfseries\color{white}] (0,0) {\thepage};
	\end{tikzpicture}}
\fancyhead[LE]
{\begin{tikzpicture}[baseline, every node/.style={minimum size=8mm, anchor=base}]
		\path node at (0,0) [shape=circle, fill=cyan!60,font=\bfseries\color{white}] (0,0) {\thepage}
		node at (1,0) [shape=circle, fill=cyan!40] (0,0) {\Z}
		node at (2,0) [shape=circle, fill=cyan!20] (0,0) {\Z};
	\end{tikzpicture}}
%================================================================
\fancyhead[LO]{\textcolor{mybluei} \rightmark%
	\begin{tikzpicture}[overlay,remember picture]
		\path[fill=\BoxColor] (current page.north east) rectangle ($(current page.south east)-(15mm,0mm)$);		
		\node[inner sep=-0pt,rectangle] at ($ (current page.east)-(10mm,0mm) $)
		{
			\rotatebox{-90}{\textcolor{white}{\bfseries\scshape \leftmark}}
		};%奇数页
		%
		\fill[\BoxColor] (current page.north west) rectangle ++(0.8,-8.8);
		%
		\foreach \i [count=\j starting from 1]in {1, 3/2, 11/6, 25/12, 137/60, 49/20, 363/140, 761/280, 7129/2520, 7381/2520}
		{
			\fill[\BoxColor] ([yshift={-8.1cm-2*\i*1cm}]current page.north west) rectangle ++(0.8,1/\j);
		}
	\end{tikzpicture}}

\fancyhead[RE]{\textcolor{mybluei}\leftmark%
	\begin{tikzpicture}[overlay,remember picture]
		\path[fill=\BoxColor] (current page.north west) rectangle ($(current page.south west)+(15mm,0mm)$);		
		\node[fill=\BoxColor,inner sep=0pt,rectangle]
		at ($ (current page.west)+(10mm,0mm) $)
		{\rotatebox{90}{\parbox{\paperheight}{%
					\centering\textcolor{white}{\bfseries\scshape \leftmark }}}};%偶数页
		\fill[\BoxColor] (current page.north east) rectangle ++(-0.8,-8.8);
		\foreach \i [count=\j starting from 1]in {1, 3/2, 11/6, 25/12, 137/60, 49/20, 363/140,
			761/280, 7129/2520, 7381/2520}
		{
			\fill[\BoxColor] ([yshift={-8.1cm-2*\i*1cm}]current page.north east)
			rectangle ++(-0.8,1/\j);}
	\end{tikzpicture}}
%=====================================================
\renewcommand{\headrulewidth}{1pt}
\addtolength{\headheight}{2.5pt}
\renewcommand{\footrulewidth}{1pt}
\fancyfoot[LE,RO]{\footnotesize\bfseries\itshape Foot 1}
\fancyfoot[C]{\footnotesize\bfseries Foot 2}
\fancyfoot[RE,LO]{\footnotesize\bfseries\itshape Foot 3}
%======================================
\makeatletter
\renewcommand{\cleardoublepage}{
	\clearpage\ifodd\c@page\else
	\hbox{}
	\vspace*{\fill}
	\thispagestyle{empty}
	\newpage
	\fi}
\makeatother
