\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{MyBook}[2025/09/25， v 0.0.1]
\newcommand{\BaseBook}{ctexbook}
% --- 动态传递用户在 \documentclass[...] 中给出的选项到基础类 (ctexbook) ---

% 其它未显式声明的选项直接传给基础类
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{\BaseBook}}

% 处理本类选项
\ProcessOptions\relax



% 现在加载基础类并应用累积的选项
\LoadClassWithOptions{\BaseBook}
% 页面设置
\RequirePackage{geometry}
\geometry{
    a4paper,
    inner=3cm,
    outer=2.2cm,
    top=2.5cm,
    bottom=3cm,
    marginparsep=0.2cm,
    marginparwidth=2cm
}


\RequirePackage[dvipsnames,svgnames,x11names,table]{xcolor}
\RequirePackage{xparse}
\RequirePackage{amssymb} % for robust math symbols like \hookleftarrow
\RequirePackage{graphicx}
\graphicspath{{images/}}
\RequirePackage{calc,varwidth}
\RequirePackage{tikz}
% 去重并按常用优先级整理 TikZ 库顺序
\usetikzlibrary{backgrounds,intersections,positioning,fit,shapes,fadings,decorations.pathmorphing,graphs,quotes,angles,calc,through}

\pgfdeclarelayer{background}
\pgfdeclarelayer{foreground}
\pgfsetlayers{background,main,foreground}

% 自定义配色（用于有背景图时的高对比度主题）
% 将章节强调色切换为酒红色主题
\definecolor{chapAccent}{HTML}{800020}% 酒红色，增强对比
\definecolor{titleBackdrop}{HTML}{000000}% 纯黑背景，配合半透明
\definecolor{wineRed}{HTML}{800020}% 酒红色（勃艮第色系）

\RequirePackage[most]{tcolorbox}
\tcbuselibrary{xparse,hooks,skins,breakable,theorems}
% 章节标题设置
%%%%%%%%%% 让章首页不显示页码
\let\ps@plain\ps@empty
%数字字体
\newfont{\font@eurb}{eurb10 scaled 4000}
\newfont{\font@cmb}{cmb10 scaled 1300} %pplr9d D:\texlive\2018\texmf-dist\fonts\type1\public\amsfonts\cm cmb10
\newfont{\font@pplr}{pplr9d scaled 4000}
\newcommand{\chap@numfont}{\font@eurb}



\def\chap@image{} % 章节背景图片文件名
\newcommand{\setchapterimage}[1]{\def\chap@image{#1}}%
% 计算版心左边界相对 page.north west 的水平偏移（twoside 兼容）
\newdimen\chap@textarealeft
\newcommand{\chap@settextarealeft}{%
  \if@twoside
    \ifodd\c@page
      \chap@textarealeft=\dimexpr1in+\oddsidemargin+\hoffset\relax
    \else
      \chap@textarealeft=\dimexpr1in+\evensidemargin+\hoffset\relax
    \fi
  \else
    \chap@textarealeft=\dimexpr1in+\oddsidemargin+\hoffset\relax
  \fi
}
% 章节标题后垂直间距（有/无背景两档，固定值）
\newlength{\chap@vsp@with}
\newlength{\chap@vsp@without}
\setlength{\chap@vsp@with}{7.5cm}
\setlength{\chap@vsp@without}{4cm}
% 章号色块宽度与标题间隙（固定值）
\newlength{\chap@num@width}
\setlength{\chap@num@width}{2.5cm}
\newlength{\chap@title@gap}
\setlength{\chap@title@gap}{1cm}
% 紧致包裹模式：测量章号自然宽度，并提供固定内边距
\newsavebox{\chap@num@box}
\newlength{\chap@num@pad}
\setlength{\chap@num@pad}{6pt}
\newlength{\chap@num@actualwidth}
% 标题在有背景图时的半透明底与内边距（固定值）
\newlength{\chap@title@pad}
\setlength{\chap@title@pad}{6pt}
\def\chap@title@opacity{0.58}
\def\chap@title@nodeopts{}
\AtBeginDocument{\tikzset{chapTitleOnBG/.style={inner sep=\chap@title@pad, fill=titleBackdrop, fill opacity=\chap@title@opacity, rounded corners=4pt}}}
\newif\ifchap@title@backdrop
\chap@title@backdroptrue
\newsavebox{\chap@bgbox}
\newlength{\chap@bg@height}
\newlength{\chap@bg@halfheight}
\newlength{\chap@bg@belowgap} % 图像底部到正文的安全间距
\setlength{\chap@bg@belowgap}{12pt}
\newlength{\chap@post@vsp}    % 章节标题后实际使用的间距
% 章节标题后垂直间距的经验补偿（用于基于背景图高度的动态计算，默认 1.5cm）
\newlength{\chap@post@comp}
\setlength{\chap@post@comp}{1.5cm}
\newcommand{\setchapterpostcomp}[1]{\setlength{\chap@post@comp}{#1}}
% 章号（chapnum）垂直位置微调量和内部计算用的 y 偏移
\newlength{\chap@chapnum@yadjust}
\setlength{\chap@chapnum@yadjust}{0pt}
\newlength{\chap@chapnum@yshift}
% 无背景时章号基准纵向位移（默认 -1in-\voffset-\headsep，可调）
\newlength{\chap@chapnum@basey}
\setlength{\chap@chapnum@basey}{-\dimexpr1in-\voffset-\headsep\relax}
% 无编号章标题的 y 偏移（与有编号章的 chapnum 位置计算一致，单独一条长度以避免重复定义报错）
\newlength{\chap@title@yshift}
% 无编号章在“无背景图”时用于垂直居中的“虚拟高度”（可调）
\newlength{\chap@schapter@virtualheight}
\setlength{\chap@schapter@virtualheight}{7cm}
% 用户接口：设置无编号章在无背景图时用于计算中心的虚拟高度
\newcommand{\setschaptervirtualheight}[1]{\setlength{\chap@schapter@virtualheight}{#1}}
% 无编号章标题的字体（可配置），默认使用黑体大号加粗
\newcommand{\chap@schapter@font}{\Huge\bfseries\heiti}
\newcommand{\setschapterfont}[1]{\renewcommand{\chap@schapter@font}{#1}}
% 无编号章标题的字距与行距（可配置）
% 字距：fontspec 的 LetterSpace，单位为千分 em（整数），默认 0
\def\chap@schapter@letterspace{0}
\newcommand{\setschapterletterspace}[1]{\def\chap@schapter@letterspace{#1}}
% 行距：\linespread 因子（相对字号的倍数），默认 1.15
\def\chap@schapter@linespread{1.15}
\newcommand{\setschapterlinespread}[1]{\def\chap@schapter@linespread{#1}}
% 编号章标题的字体（可配置），默认 \Huge\bfseries
\newcommand{\chap@chapter@font}{\fontsize{36pt}{40pt}\selectfont\bfseries}
\newcommand{\setchaptertitlefont}[1]{\renewcommand{\chap@chapter@font}{#1}}
% 用户接口：细调章号在竖直方向上的位置（正值下移，负值上移）
\newcommand{\setchapnumyadjust}[1]{\setlength{\chap@chapnum@yadjust}{#1}}
% 用户接口：设置无背景时章号的基准纵向位移
\newcommand{\setchapnumbasey}[1]{\setlength{\chap@chapnum@basey}{#1}}
% 计算背景图在以 width=\paperwidth 缩放后的高度
\newcommand{\chap@calc@bgheight}{%
  \ifx\chap@image\@empty
    \setlength{\chap@bg@height}{0pt}% 无图
  \else
    \sbox{\chap@bgbox}{\includegraphics[width=\paperwidth]{\chap@image}}%
    \setlength{\chap@bg@height}{\dimexpr\ht\chap@bgbox+\dp\chap@bgbox\relax}% 盒子高度+深度
  \fi
}
% \@makechapterhead 用于生成有编号章节的标题。
\let\old@makechapterhead\@makechapterhead
\renewcommand{\@makechapterhead}[1]{%
  \ifx\chap@image\@empty\old@makechapterhead{#1}\else
  \begin{tikzpicture}[remember picture,overlay]
    \node at (current page.north west)
  {
    \begin{tikzpicture}[remember picture,overlay]
        \node[on background layer,anchor=north west,inner sep=0pt,opacity=1.0] at (0,0){\includegraphics[width=\paperwidth]{\chap@image}};        
    \end{tikzpicture}
  };
    % 有背景：更强对比的深色主题 + 半透明黑底衬托标题
    \def\chap@num@fill{chapAccent}
    \def\chap@num@opacity{0.92}
    \def\chap@num@textcolor{white}
    \def\chap@num@draw{white}
    \def\chap@num@drawopacity{0.35}
  % 标题白字 + 半透明黑底，圆角与内边距
  \def\chap@title@textcolor{white}
  \def\chap@title@nodeopts{}%{chapTitleOnBG}%不启用默认黑底
  % 预计算版心左边界相对 page.north west 的水平偏移
  \chap@settextarealeft
  % 计算“紧贴”宽度：章号自然宽度 + 2*内边距
  \sbox{\chap@num@box}{{\chap@numfont \scalebox{3}{\thechapter}}}% 测量内容自身宽度
  \setlength{\chap@num@actualwidth}{\wd\chap@num@box}
  \addtolength{\chap@num@actualwidth}{2\chap@num@pad}
  % 计算章号的垂直位置：
  % - 有背景图：使其位于背景图垂直中线附近（从页面北西角向下移动 backgroundHeight/2）
  % - 无背景图：沿用原先的 -3.5cm 经验位置
  \chap@calc@bgheight
    \setlength{\chap@bg@halfheight}{0.5\chap@bg@height}
    % 使节点中心大致对齐到图片中线，因此再上移半个章号节点高度近似
    % 为简洁起见，这里不估算节点高度，仅留给用户 \chap@chapnum@yadjust 做微调
    \setlength{\chap@chapnum@yshift}{-\chap@bg@halfheight}
  \addtolength{\chap@chapnum@yshift}{\chap@chapnum@yadjust}

  % 使用带填充、圆角的 TikZ 节点直接着色，让色块紧紧包裹文字
  \node[
      anchor=west,
      text=\chap@num@textcolor,
      align=center,
      font=\chap@numfont,
      inner sep=\chap@num@pad,
      fill=\chap@num@fill,
      fill opacity=\chap@num@opacity,
    draw=\chap@num@draw,
    draw opacity=\chap@num@drawopacity,
      rounded corners=5pt
  ] (chapnum) at ([xshift=\chap@textarealeft,yshift=\chap@chapnum@yshift]current page.north west) {\scalebox{3}{\thechapter}};
  % 标题文字颜色：无背景时用黑色，有背景时用白色
  % 修正：当有黑底（带 inner sep）时，需从锚点中减去内边距，保证“可见文字”与预期间距/左边界一致
  \setlength{\@tempdima}{\chap@title@gap}% 默认间隙
    \addtolength{\@tempdima}{-\chap@title@pad}% 有黑底时左移 inner sep
  \node[anchor=west,inner sep=0mm,outer sep=0mm,font=\chap@chapter@font,text=\chap@title@textcolor,\chap@title@nodeopts] (chapname) at ($(chapnum.east)+(\the\@tempdima,0cm)$)
			{\begin{varwidth}{\dimexpr\textwidth-\chap@num@actualwidth-\chap@title@gap\relax}#1\end{varwidth}};
\end{tikzpicture}
% 使用非可丢弃的顶端垂直间距，确保正文不会顶到页面顶部
% TeX 在页面顶端会忽略普通的可丢弃垂直胶（水平无关），\vspace{...} 正属此类。
% 章首页通常从页面最上方开始排，tikz overlay 不占据版心竖直空间，需要显式、不可丢弃的垂直间距。
% \vspace* 是“不可丢弃”的版本，配合 \par 能稳定生效。
% 按有/无背景图自适应
% 计算背景图高度并动态决定章节标题后的垂直间距
\chap@calc@bgheight
  % 有图：保证正文位于“背景图底部 + 安全间距”之下，且不小于无图间距
  % 估计章节头部基准参考线到页面顶部的固定偏移：当前布局中，数值近似为 3.5cm（节点 yshift），
  % 考虑标题块高度（章号块+标题行）的一致性，取一个经验补偿 1.5cm。
  % 动态间距 = max( 无图默认间距, 背景图高度 + 安全间距 - 经验补偿 )
  \setlength{\chap@post@vsp}{\chap@vsp@without}
  \@tempdima=\chap@bg@height
  \advance\@tempdima by \chap@bg@belowgap
  % 经验补偿（默认 1.5cm），参数化可配置
  \advance\@tempdima by -\chap@post@comp
  \ifdim \@tempdima>\chap@post@vsp
    \setlength{\chap@post@vsp}{\@tempdima}
  \fi
\fi
\par\vspace*{\chap@post@vsp}
}%\@makechapterhead命令结束

% \@makeschapterhead 与 \@makechapterhead 类似，但用于无编号章节。
% 注意命令多一个's'。
\renewcommand{\@makeschapterhead}[1]{
  % 配色与标题风格（与有编号章保持一致）
  \ifx\chap@image\@empty
    \def\chap@title@textcolor{black}
    \def\chap@title@nodeopts{}
  \else
    \def\chap@title@textcolor{white}
    \ifchap@title@backdrop
      \def\chap@title@nodeopts{chapTitleOnBG}
    \else
      \def\chap@title@nodeopts{}
    \fi
  \fi
  % 计算文本区左边界与背景图高度
  \chap@settextarealeft
  \chap@calc@bgheight
  % 计算标题的垂直位置：
  % - 有背景图：取背景图中线
  % - 无背景图：以可配置的“虚拟高度”居中（默认 7cm，可用 \setschaptervirtualheight 调整）
  \ifx\chap@image\@empty
    \setlength{\chap@title@yshift}{-0.5\chap@schapter@virtualheight}
  \else
    \setlength{\chap@title@yshift}{-0.5\chap@bg@height}
  \fi
  \addtolength{\chap@title@yshift}{\chap@chapnum@yadjust}% 允许用户微调
  % 计算标题水平位置：无编号标题水平居中于正文区
  % 文本区中心 = 版心左 + 0.5*\textwidth
  \@tempdima=\chap@textarealeft
  \advance\@tempdima by .5\textwidth

  \begin{tikzpicture}[remember picture,overlay]
    \node at (current page.north west)
    {
      \begin{tikzpicture}[remember picture,overlay]
        % 背景图
        \ifx\chap@image\@empty
          % 无背景
        \else
          % 将背景图绘制在背景图层，确保标题在其之上
          \node[on background layer,anchor=north west,inner sep=0pt] at (0,0){\includegraphics[width=\paperwidth]{\chap@image}};
        \fi
      \end{tikzpicture}
    };
  % 标题节点：水平居中（anchor=center），字体/字距/行距可配置
  \node[anchor=center,inner sep=0mm,outer sep=0mm,font=\chap@schapter@font,
              text=\chap@title@textcolor,\chap@title@nodeopts]
       at ([xshift=\the\@tempdima,yshift=\chap@title@yshift]current page.north west)
    {\begingroup\linespread{\chap@schapter@linespread}\selectfont\ifnum\chap@schapter@letterspace=0\relax\else\addfontfeatures{LetterSpace=\chap@schapter@letterspace}\fi\begin{varwidth}{\textwidth}#1\end{varwidth}\endgroup};
  \end{tikzpicture}

  % 正文起始位置：与有编号章相同策略
  % - 有背景图：保证正文位于“背景图底部 + 安全间距”之下
  % - 无背景图：使用“虚拟高度”驱动，确保正文在标题块下方留有合适空白
  \chap@calc@bgheight
  \ifx\chap@image\@empty
    % 无编号且无背景：正文与标题使用“常规距离”（可单独设定）
  \setlength{\chap@post@vsp}{\chap@vsp@without}
  \else
    % 有背景：以“无编号-有背景”的默认间距为保底，与动态需求取较大值
  \setlength{\chap@post@vsp}{\chap@vsp@with}
    \@tempdima=\chap@bg@height
    \advance\@tempdima by \chap@bg@belowgap
    \advance\@tempdima by -\chap@post@comp
    \ifdim \@tempdima>\chap@post@vsp
      \setlength{\chap@post@vsp}{\@tempdima}
    \fi
  \fi
  \par\vspace*{\chap@post@vsp}
}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  开始添加用户定义  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\RequirePackage{enumitem}				
\renewcommand{\labelitemi}{$\bullet$}
\setlist[itemize]{noitemsep}

% 将有序列表的数字标签统一为酒红色（不影响正文文字）
\setlist[enumerate]{noitemsep}
%\makeatother




%代码环境定义
\RequirePackage{listings}
% 为 listings 的自动换行设置可移植的换行标记，避免在类文件中直接使用 Unicode 字符
% 使用数学符号替代：\hookleftarrow 更通用、稳定
\newcommand{\lstbreakpre}{\textcolor{wineRed}{\tiny$\hookleftarrow$}}
\newcommand{\lstbreakpost}{\textcolor{wineRed}{\tiny$\hookrightarrow$}\,}
\renewcommand*{\lstlistingname}{代码}
\renewcommand*{\lstlistingnamestyle}{\bfseries}
\lstnewenvironment{code}[1][]
{
	%	\setmonofont{mononoki}
	%	\setmonofont{mononoki}
	%staring code
	\lstset{
		language=[11]C++,
		morekeywords={UCLASS,USTRUCT,UPROPERTY,UFUNCTION,GENERATED_BODY,override,final},
		columns=fullflexible,%fixed
		basicstyle=\ttfamily\small\linespread{1.25},
		commentstyle=\color{green!70!black}\itshape,
		keywordstyle=\color{blue}\bfseries,
		numberstyle=\scriptsize\color{red!70!black}\ttfamily,
		stringstyle=\color{cyan},
		identifierstyle=\color{darkgray},
		breakautoindent,
		breakindent=4ex,
		breaklines=true,
		breakatwhitespace=false,
		showstringspaces=false,
    prebreak=\mbox{\lstbreakpre},
    postbreak=\mbox{\lstbreakpost},
    emph=[2]{override,final},
		emphstyle=[2]{\color{blue}\bfseries},		
		emph=[3]{UPROPERTY,UFUNCTION,UENUM,UCLASS,USTRUCT},
		emphstyle=[3]{\color{orange!70!black}\bfseries},
		emph=[4]{BlueprintType,EditDefaultsOnly,BlueprintReadOnly},
		emphstyle=[4]{\color{cyan!70!blue}},
    tabsize=2,
		numbers=none,
		%escapeinside=``,
    escapeinside={(*@}{@*)},
		frame=none,
		backgroundcolor=\color{gray!5},
		captionpos=t,%标题的位置
		#1
	}
}
{%end code
%可以在这里自定义命令
}


% 内联代码命令：提供“安全版”和“高亮版”两种：
% 1) \ci[<style>]{...} 安全版：可用于标题、目录等“移动参数”中；不使用 verbatim，整段等宽、酒红色。
% 2) \cii[<listings-keys>]|...| 高亮版：基于 \lstinline，提供“关键字蓝、其余酒红”的混合风格，仅用于普通正文内。

% 安全版：可用于 moving arguments（如 \section、\caption、\item 等）
% 说明：xparse 定义的命令为健壮（robust），这里避免 verbatim 类实现，保证在写入 .aux/.toc 时不出错。
\NewDocumentCommand{\ci}{O{} m}{%
  {\ttfamily\color{blue}#2}% 可选项 #1 预留，当前不参与样式（保持兼容）
}

% 高亮版：原来的 verbatim 方案，改名为 \cii；使用分隔符调用：\cii[<keys>]|code|
\NewDocumentCommand{\cii}{O{} v}{%
  \lstinline[
    language=C++,
    breaklines,
    breakatwhitespace=false,
    basicstyle=\ttfamily\color{black},
    keywordstyle=\bfseries\color{blue},
    identifierstyle=\color{blue},
    stringstyle=\color{wineRed},
    commentstyle=\color{green},
    #1
  ]{#2}%
}

\newcommand{\cppsign}{$C^{++}$}
\newcommand{\cpp}[1][11]{$C^{++ \mathbf{#1}}$}
\newcommand{\cssign}{$C{\#}$}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  结束添加用户定义  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%marker环境
\newtcolorbox{marker}[1][]{enhanced,before skip=2mm,
	after skip=3mm,fontupper=\rmfamily,
	boxrule=0.4pt,left=5mm,right=2mm,top=1mm,bottom=1mm,
	colback=yellow!50,colframe=yellow!20!black,
	sharp corners,rounded corners=southeast,
	arc is angular,arc=3mm,underlay={%
		\path[fill=tcbcolback!80!black] ([yshift=3mm]interior.south east)--++(-0.4,-0.1)--++(0.1,-0.2);
		\path[draw=tcbcolframe,shorten <=-0.05mm,shorten >=-0.05mm] ([yshift=3mm]interior.south east)--++(-0.4,-0.1)--++(0.1,-0.2);
		\path[fill=yellow!50!black,draw=none] (interior.south west) rectangle node[white]{\Huge\bfseries !} ([xshift=4mm]interior.north west);
	},
	drop fuzzy shadow,#1
}

%%设置新字体
%%定义带圈数字命令
\newfontfamily{\nmfont}[Extension=.otf,Path=./fonts/]{circlenumber}%

\newcommand{\quan}[1]{{\nmfont \symbol{#1}}}
\newcommand{\kk}[1]{\quan{\numexpr32+#1}}%\kk{<参数范围1-95>}96、97、98、99分别用\quan{196} \quan{197} \quan{199} \quan{201}

%脚注使用带圈数字
\newcommand*\kkctr[1]{%
  \protect\kk{\number\numexpr\value{#1}\relax}}
\renewcommand*\thefootnote{\textcolor{wineRed}{\kkctr{footnote}}}

%%无悬挂脚注格式
\renewcommand\@makefntext[1]{%
\setlength\parindent{2\ccwd}\selectfont\@thefnmark\ #1}%

%修改\part，使其不分页
\def\@endpart{%
	\thispagestyle{empty}
  \vskip40\p@%
  \@afterheading
 }
 
 
 % ---- Pandoc compatibility stubs ----
 % Pandoc EPUB->LaTeX conversion may emit wrappers like \pandocbounded{...} to manage float/graphic layout.
 % Define a no-op wrapper so the included content passes through unchanged.
 % Also guard against multiple definitions if user later provides custom behavior.
 \providecommand{\pandocbounded}[1]{#1}
 % Some pandoc outputs prefix sectional anchors with \protect\phantomsection\label{...}. Both macros exist in hyperref;
 % if hyperref not loaded yet, \phantomsection is undefined -> we either load hyperref or stub it.
 % For now provide a lightweight stub; if user later loads hyperref the real definition overrides this.
 \providecommand{\phantomsection}{}