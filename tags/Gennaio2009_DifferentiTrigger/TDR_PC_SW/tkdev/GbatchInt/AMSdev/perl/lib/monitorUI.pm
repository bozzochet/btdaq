#  $Id: monitorUI.pm,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
package monitorUI;
use Error qw(:try);
use Gtk;
use Gtk::Atoms;
use Carp;
use strict;
use lib::Monitor;
@monitorUI::EXPORT =qw(new);

my ($book_open, $book_open_mask);

my @book_open_xpm = (
"16 16 4 1",
"       c None s None",
".      c black",
"X      c #808080",
"o      c white",
"                ",
"  ..            ",
" .Xo.    ...    ",
" .Xoo. ..oo.    ",
" .Xooo.Xooo...  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
"  .Xoo.Xoo..X.  ",
"   .Xo.o..ooX.  ",
"    .X..XXXXX.  ",
"    ..X.......  ",
"     ..         ",
"                ");

my @book_open_r_xpm = (
"16 16 4 1",
"       c None s None",
".      c black",
"X      c #808080",
"o      c red",
"                ",
"  ..            ",
" .Xo.    ...    ",
" .Xoo. ..oo.    ",
" .Xooo.Xooo...  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
"  .Xoo.Xoo..X.  ",
"   .Xo.o..ooX.  ",
"    .X..XXXXX.  ",
"    ..X.......  ",
"     ..         ",
"                ");

my @book_open_g_xpm = (
"16 16 4 1",
"       c None s None",
".      c black",
"X      c #808080",
"o      c green",
"                ",
"  ..            ",
" .Xo.    ...    ",
" .Xoo. ..oo.    ",
" .Xooo.Xooo...  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
"  .Xoo.Xoo..X.  ",
"   .Xo.o..ooX.  ",
"    .X..XXXXX.  ",
"    ..X.......  ",
"     ..         ",
"                ");

my @book_open_y_xpm = (
"16 16 4 1",
"       c None s None",
".      c black",
"X      c #808080",
"o      c yellow",
"                ",
"  ..            ",
" .Xo.    ...    ",
" .Xoo. ..oo.    ",
" .Xooo.Xooo...  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
"  .Xoo.Xoo..X.  ",
"   .Xo.o..ooX.  ",
"    .X..XXXXX.  ",
"    ..X.......  ",
"     ..         ",
"                ");


my @green_xpm = (
"16 16 6 1",
"       c None s None",
".      c black",
"X      c green",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"     XXXX       ",
"    XXXXXX      ",
"   XXXXXXXX     ",
"  XXXXXXXXXX    ",
" XXXXXXXXXXXX   ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
" XXXXXXXXXXXX   ",
"  XXXXXXXXXX    ",
"   XXXXXXXX     ",
"    XXXXXX      ",
"     XXXX       ",
"                ");
my @yellow_xpm = (
"16 16 6 1",
"       c None s None",
".      c black",
"X      c yellow",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"     XXXX       ",
"    XXXXXX      ",
"   XXXXXXXX     ",
"  XXXXXXXXXX    ",
" XXXXXXXXXXXX   ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
" XXXXXXXXXXXX   ",
"  XXXXXXXXXX    ",
"   XXXXXXXX     ",
"    XXXXXX      ",
"     XXXX       ",
"                ");
my @red_xpm = (
"18 16 6 1",
"       c None s None",
".      c black",
"X      c red",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"     XXXXXX       ",
"    XXXXXXXX      ",
"   XXXXXXXXXX     ",
"  XXXXXXXXXXXX    ",
" XXXXXXXXXXXXXX   ",
"X##X###X###X###X  ",
"X#XXX#XX#X#X#X#X  ",
"X##XX#XX#X#X#X#X  ",
"XX#XX#XX#X#X##XX  ",
" ##XX#XX###X#XXX  ",
"  XXXXXXXXXX#XX   ",
"   XXXXXXXXXXX    ",
"    XXXXXXXXX     ",
"     XXXXXXX      ",
"                  ");

my @all_xpm=[
@green_xpm,
@yellow_xpm,
@red_xpm,
             ];

my @book_closed_g_xpm = (
"16 16 6 1",
"       c None s None",
".      c black",
"X      c green",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"       ..       ",
"     ..XX.      ",
"   ..XXXXX.     ",
" ..XXXXXXXX.    ",
".ooXXXXXXXXX.   ",
"..ooXXXXXXXXX.  ",
".X.ooXXXXXXXXX. ",
".XX.ooXXXXXX..  ",
" .XX.ooXXX..#O  ",
"  .XX.oo..##OO. ",
"   .XX..##OO..  ",
"    .X.#OO..    ",
"     ..O..      ",
"      ..        ",
"                ");


my @book_closed_r_xpm = (
"16 16 6 1",
"       c None s None",
".      c black",
"X      c red",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"       ..       ",
"     ..XX.      ",
"   ..XXXXX.     ",
" ..XXXXXXXX.    ",
".ooXXXXXXXXX.   ",
"..ooXXXXXXXXX.  ",
".X.ooXXXXXXXXX. ",
".XX.ooXXXXXX..  ",
" .XX.ooXXX..#O  ",
"  .XX.oo..##OO. ",
"   .XX..##OO..  ",
"    .X.#OO..    ",
"     ..O..      ",
"      ..        ",
"                ");


my @book_closed_y_xpm = (
"16 16 6 1",
"       c None s None",
".      c black",
"X      c yellow",
"o      c red",
"O      c #808080",
"#      c white",
"                ",
"       ..       ",
"     ..XX.      ",
"   ..XXXXX.     ",
" ..XXXXXXXX.    ",
".ooXXXXXXXXX.   ",
"..ooXXXXXXXXX.  ",
".X.ooXXXXXXXXX. ",
".XX.ooXXXXXX..  ",
" .XX.ooXXX..#O  ",
"  .XX.oo..##OO. ",
"   .XX..##OO..  ",
"    .X.#OO..    ",
"     ..O..      ",
"      ..        ",
"                ");


sub Edit{
}

sub new{
my %fields=(
            window=>undef,
            clist=>{
#               Producer_ActiveHosts=>undef,
#               Producer_ActiveClients=>undef,
#               Producer_Runs=>undef,
#               Server=>undef,
               DiskUsage=>undef,
               },
               pixmap=>[],
               mask=>[],
               pixmapb=>[],
               maskb=>[],
               notebookw=>undef,
               statusbar=>undef,
               notebook=>[0,0,0,0,0],
               current_page=>undef,
            );
    my $type=shift;
    my $self={
        %fields,
    };




Gtk->init;



$self->{window}  = new Gtk::Widget    "Gtk::Window",
                "GtkWindow::type"                 =>      -toplevel,
                "GtkWindow::title"                =>      "AMS Production Monitor",
                "GtkWindow::allow_grow"           =>      1,
                "GtkWindow::allow_shrink"         =>      1,
                "GtkWindow::auto_shrink"         =>      0,
                "GtkContainer::border_width"      =>      10;


my $vbox= new_child {$self->{window}} "Gtk::VBox" ;

#my $menubar = new Gtk::MenuBar;
#$vbox->pack_start($menubar, 0, 1, 0);
#my $menu = new Gtk::Menu;    
#my $menuitem=new Gtk::MenuItem("Update Status");
#$menuitem->signal_connect('activate',\&Update);
#$menu->append($menuitem);
#$menuitem=new Gtk::MenuItem("Quit");
#$menuitem->signal_connect('activate',sub{exit();});
#$menu->append($menuitem);
#
#$menuitem = new Gtk::MenuItem("File");
#$menuitem->set_submenu($menu);
#$menubar->append($menuitem);
#$menuitem = new Gtk::MenuItem("Help");
#$menubar->append($menuitem);
#
#$menuitem = new Gtk::MenuItem("About");
#$menubar->append($menuitem);

my @item_factory_entries = (
	["/_File",	undef,	0,	"<Branch>"],
	["/File/tearoff1",	undef,	0,	"<Tearoff>"],
	["/File/_Update Status",	"<alt>U",	1],
	["/File/sep1",	undef,	0,	"<Separator>"],
	["/File/_Force Check",	"<alt>C",	6],
	["/File/sep1",	undef,	0,	"<Separator>"],
	["/File/_Reset Failed Runs",	"<alt>R",	7],
	["/File/Reset _Hosts",	"<alt>H",	8],
	["/File/_Delete Runs",	"<alt>D",	9],
	["/File/_Restore Runs",	"<alt>R",	10],
	["/File/R_ebuild DB",	"<alt>E",	11],
	["/File/Dele_te Failed DST",	"<alt>T",	12],
	["/File/Delete _Validated DST",	"<alt>V",	13],
	["/File/F_inish Failed Runs",	"<alt>I",	14],
	["/File/sep1",	undef,	0,	"<Separator>"],
	{
		'path' => "/File/_Quit", 
		'accelerator' => "<alt>Q",	
		'action' => 5,
		'type' => '<Item>'
	},
      ["/_Preferences",	undef,	0,	"<Branch>"],
      ["/Preferences/_RegisteredToServers", "<alt>R",	2,	"<ToggleItem>"],
      ["/Preferences/_UpdatesViaDB", "<alt>D",	3,	"<ToggleItem>"],
      ["/_Help",	undef,	0,	"<Item>"],
      ["/_About",	undef,	0,	"<Item>"],
                        );

		my $accel_group = new Gtk::AccelGroup;
		my $item_factory = new Gtk::ItemFactory('Gtk::MenuBar', "<main>", $accel_group);
		
		$accel_group->attach($self->{window});
		foreach (@item_factory_entries) {
			$item_factory->create_item($_, \&item_factory_cb);
		}
$vbox->pack_start($item_factory->get_widget('<main>'), 0, 0, 0);
my $hbox= new Gtk::HBox 1, 0;
$vbox->pack_start($hbox,1,1,0);
#my $bquit =   new Gtk::Widget "Gtk::Button",
#  "GtkButton::label"              =>      "Quit",
#  "GtkObject::signal::clicked"     =>      sub{exit();},
#  "GtkWidget::visible"              =>      1;
#my $bupdate =   new Gtk::Widget "Gtk::Button",
#  "GtkButton::label"                =>      "Update Status",
#  "GtkObject::signal::clicked"      =>      \&Update,
#  "GtkWidget::visible"              =>      1;
#my $bedit =   new Gtk::Widget "Gtk::Button",
#  "GtkButton::label"                =>      "Edit",
#  "GtkObject::signal::clicked"      =>      \&Edit,
#  "GtkWidget::visible"              =>      1;
#$hbox->pack_start($bquit,1,1,0);
#$hbox->pack_start($bupdate,1,1,0);
#$hbox->pack_start($bedit,1,1,0);

		
my $separator = new Gtk::HSeparator();
$vbox->pack_start( $separator, 0, 1, 10 );


my $notebook = new Gtk::Notebook;
$self->{notebookw}=$notebook;
$notebook->signal_connect( 'switch_page', \&notebook_page_switch );
$notebook->set_tab_pos(-top);
$vbox->pack_start($notebook, 1, 1, 0);
$notebook->border_width(10);
$notebook->realize;
my $transparent;

		
    my $mybless=bless $self,$type;
    if(ref($monitorUI::Singleton)){
        croak "Only Single monitorUI Allowed\n";
    }
    $monitorUI::Singleton=$mybless;

 ($book_open, $book_open_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_open_g_xpm);
$self->{pixmapbo}[0]=$book_open;
$self->{maskbo}[0]=$book_open_mask;
 ($book_open, $book_open_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_open_y_xpm);
$self->{pixmapbo}[1]=$book_open;
$self->{maskbo}[1]=$book_open_mask;
 ($book_open, $book_open_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_open_r_xpm);
$self->{pixmapbo}[2]=$book_open;
$self->{maskbo}[2]=$book_open_mask;
my ($book_closed, $book_closed_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_closed_g_xpm);
$self->{pixmapb}[0]=$book_closed;
$self->{maskb}[0]=$book_closed_mask;
($book_closed, $book_closed_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_closed_y_xpm);
$self->{pixmapb}[1]=$book_closed;
$self->{maskb}[1]=$book_closed_mask;
($book_closed, $book_closed_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_closed_r_xpm);
$self->{pixmapb}[2]=$book_closed;
$self->{maskb}[2]=$book_closed_mask;


$self->notebook_create_pages($notebook, 0, 4);

my $statusbar = new Gtk::Statusbar;
$vbox->pack_end($statusbar, 1, 1, 0);
$self->{statusbar}=$statusbar;
if (not $Monitor::Singleton->{ok}){
    $statusbar->push(1," Unable to Connect Servers");
}else{
    $statusbar->push(1," Connected to Servers");
}		
Gtk->timeout_add(900000,\&Update);
Gtk->timeout_add(3600000,\&ReCreate);
return $mybless;
}


sub notebook_page_switch {

        my($widget, $new_page, $page_num)  = @_;

#	print "switch page $page_num\n";

	my $old_page = $widget->cur_page();
	if ( defined $old_page ) {
		if ($new_page == $old_page) {
                        
			return;
		}
	}
       
	my $new_page_tab_pixmap = ($new_page->tab_label->children)[0]->widget;
	my $new_page_menu_pixmap = ($new_page->menu_label->children)[0]->widget;

#	$new_page_tab_pixmap->set( $book_open, $book_open_mask );
#	$new_page_menu_pixmap->set( $book_open, $book_open_mask );
        my $Singleton=$monitorUI::Singleton;	
                my $num=$Singleton->{notebook}[$page_num];
		$new_page_tab_pixmap->set( $Singleton->{pixmapbo}[$num],  $Singleton->{maskbo}[$num]  );
		$new_page_menu_pixmap->set( $Singleton->{pixmapbo}[$num],  $Singleton->{maskbo}[$num]  );
	if ( defined $old_page  and defined $Singleton->{current_page}) {
		my $old_page_tab_pixmap = ($old_page->tab_label->children)[0]->widget;
		my $old_page_menu_pixmap = ($old_page->menu_label->children)[0]->widget;
                $num=$Singleton->{notebook}[$Singleton->{current_page}];
		$old_page_tab_pixmap->set( $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num]  );
		$old_page_menu_pixmap->set( $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num]  );
                                         
            }
        $Singleton->{current_page}=$page_num;
    }


sub notebook_update_pages {
        my( $widget, $start, $end)=@_;
	for my $i ( $start .. $end ) {
            $widget->set_page($i);
            my $new_page=  $widget->cur_page();
	my $new_page_tab_pixmap = ($new_page->tab_label->children)[0]->widget;
	my $new_page_menu_pixmap = ($new_page->menu_label->children)[0]->widget;
        my $Singleton=$monitorUI::Singleton;	
                my $num=$Singleton->{notebook}[$Singleton->{current_page}];
		$new_page_tab_pixmap->set( $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num]  );
		$new_page_menu_pixmap->set( $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num]  );
            
        }
            $widget->set_page(0);
}
sub notebook_create_pages {
	  my( $self, $notebook, $start, $end ) = @_;
	my(
		$child,
		$label,
		$entry,
		$box,
		$hbox,
		$label_box,
		$menu_box,
		$button,
		$pixwid,
		$i,
		$buffer
	);
	
	for $i ( $start .. $end ) {
            if($i==0){
                $buffer="Producer Processes";
              }elsif($i==1){
                  $buffer="Producer I/O";
              }elsif($i==2){
                  $buffer="Server";
              }elsif($i==3){
                 $buffer="DiskUsage";
              }elsif($i==4){
                 $buffer="Control";
              } 	

# 
            $child = new Gtk::VBox (1,0);
#            my $child2 = new Gtk::VBox (1,0);
#            $child->pack_start($child2,0,1,0);
            $child->show_all();
                my $Singleton=$monitorUI::Singleton;	
		$label_box = new Gtk::HBox 0, 0;
                my $num=$Singleton->{notebook}[$i];
		my $pixwid = new Gtk::Pixmap $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num];
		$label_box->pack_start($pixwid, 0, 1, 0);
		$pixwid->set_padding(3, 1);
		$label = new Gtk::Label $buffer;
		$label_box->pack_start($label, 0, 1, 0);
		show_all $label_box;
                $menu_box = new Gtk::HBox( 0, 0 );
		$pixwid = new Gtk::Pixmap $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num];
                $menu_box ->pack_start( $pixwid, 0, 1, 0 );
                $pixwid->set_padding( 3, 1);
                $label = new Gtk::Label( $buffer) ;
                $menu_box->pack_start( $label, 0, 1, 0 );
                $menu_box->show_all();
		
		
		$notebook->append_page_menu($child, $label_box, $menu_box);
            $child->set_usize(1100,400);
          my (@titles,$policy_x,$policy_y);
            if($i==0){
                	@titles = (
	    "HostName",
	    "Running Pr",
	    "Allowed Pr",
	    "Failed Pr",
	    " Ntuples",
            "EventTags ",
	    " Events ",
	    "  % of Total",
	    " Warnings  ",
	    " Errors  ",
	    " CPU/Event ",
	    " Efficiency",
	    "Status ",
	);

        $policy_x="automatic";
        $policy_y="automatic";
                $buffer="Producer_ActiveHosts";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,18,@titles);

                        $#titles=-1;
                	@titles = (
	    "ID",
	    "HostName",
	    "Mips",
	    "Process ID",
	    "Start Time",
	    "LastUpdate Time",
	    "TimeOut",
            "Run",
            "Name",
            "Status",
	);

        $policy_x='automatic';
        $policy_y='automatic';
                $buffer="Producer_ActiveClients";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,18,@titles);
              }elsif($i==1){

                        $#titles=-1;
                	@titles = (
	    "Run",
	    "Time",
	    "First Event",
	    "Last Event",
	    "Priority ",
	    "History ",
	    "Fails ",
	    "Status ",
	);

        $policy_x='automatic';
        $policy_y='automatic';
                $buffer="Producer_Runs";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,16,@titles);

                        $#titles=-1;
                	@titles = (
	    "Run",
	    "Time",
	    "First Event",
	    "Last Event",
	    "Name",
            "CRC",
            "Size(MB)",
	    "Status ",
	);

        $policy_x='automatic';
        $policy_y='automatic';
                $buffer="Producer_Ntuples";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,16,@titles);

              }elsif($i==2){

                        $#titles=-1;
                	@titles = (
	    "ID",
	    "HostName",
	    "Mips",
	    "Process ID",
	    "Start Time",
	    "LastUpdate Time",
	    "TimeOut",
            "Status",
	);

        $policy_x='automatic';
        $policy_y='automatic';
                $buffer="Server_ActiveClients";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,20,@titles);
                        $#titles=-1;
                	@titles = (
	    "ID",
	    "HostName",
	    "Mips",
	    "Process ID",
	    "Start Time",
	    "LastUpdate Time",
	    "TimeOut",
            "Status",
	);

        $policy_x='automatic';
        $policy_y='automatic';
                $buffer="DBServer_ActiveClients";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,20,@titles);
                        $#titles=-1;
                	@titles = (
	    "ID",
	    "HostName",
	    "Mips",
	    "Process ID",
	    "Start Time",
	    "LastUpdate Time",
	    "TimeOut",
            "Status",
	);

        $policy_x='automatic';
        $policy_y='automatic';
                $buffer="Monitor_ActiveClients";
#                create_frame ($child,$self,$buffer,$policy_x,$policy_y,20,@titles);
              }elsif($i==3){
	@titles = (
	    "FileSystem",
	    "Total (Mbytes)",
	    "Free (Mbytes)",
	    "% Free",
	    "OK",
	);
        $policy_x='automatic';
        $policy_y='automatic';
                 $buffer="DiskUsage";
              create_frame ($child,$self,$buffer,$policy_x,$policy_y,20,@titles);

              }elsif($i==4){
#
# Control page
#
                  my $cframe=new Gtk::Frame("Control");
                  my $lframe=new Gtk::Frame("List");
                  my $aframe=new Gtk::Frame("Action");
                  $child->pack_start($cframe,0,1,0);
                  $child->pack_start($lframe,0,1,0);
                  $child->pack_start($aframe,0,1,0);


		my $scrolled_win = new Gtk::ScrolledWindow(undef, undef);
		$scrolled_win->set_policy("automatic", "automatic");
                  $scrolled_win->{clist}=undef;
                $lframe->add($scrolled_win); 
                  my $rb_p= new Gtk::RadioButton "Producer";
                  my $rb_s= new Gtk::RadioButton "Server", $rb_p ;
                  $rb_p->set_active(1);  
                  my $hbox=new Gtk::HBox(1,1);
                  my $vbox= new Gtk::VBox(1,1);
                  $cframe->add($hbox);
                  $hbox->pack_start($vbox,0,0,0);
                  $vbox->pack_start($rb_p,0,0,0);
                  $vbox->pack_start($rb_s,0,0,0);
                  my $list_scroller = new Gtk::ScrolledWindow(undef, undef);
                  $list_scroller->set_policy(-automatic, -automatic);
                  $list_scroller->border_width(5);
                  my $list=new Gtk::List;
                  $list->set_selection_mode(-single);
                  $list->signal_connect( "select_child" => sub {
                      show_sample($_[1]->{name},$scrolled_win);
                  } );
                  update_list($list, "Producer");
                  $list_scroller->add_with_viewport($list);
                  $hbox->pack_start($list_scroller,1,1,0);            
                  $rb_s->signal_connect( "clicked" => sub {
                      update_list($list, "Server");});
                  $rb_p->signal_connect( "clicked" => sub {
                      update_list($list, "Producer");});

                  my $abutton= new Gtk::Button("Add");
                  my $rbutton= new Gtk::Button("Remove");
                  my $ebutton= new Gtk::Button("Replace");
                  $abutton->signal_connect( 'clicked', \&ares,"Create",$scrolled_win);
                  $ebutton->signal_connect( 'clicked', \&ares,"Update",$scrolled_win);
                  $rbutton->signal_connect( 'clicked', \&ares,"Delete",$scrolled_win);
#                  $sbutton->signal_connect( 'clicked', \&ares,"Send",$scrolled_win);
                  my $ahbox=new Gtk::HBox(1,10);
    $ahbox->pack_start($abutton,1,1,0);
    $ahbox->pack_start($ebutton,1,1,0);
    $ahbox->pack_start($rbutton,1,1,0);
#    $ahbox->pack_start($sbutton,1,1,0);
                  $aframe->add($ahbox);

              }

        }
        my $Singleton=$monitorUI::Singleton;	
            
     my($pixmap, $mask) =create_from_xpm_d Gtk::Gdk::Pixmap( $Singleton->{window}->window, undef,  @green_xpm );
          $self->{pixmap}[0]=$pixmap;
          $self->{mask}[0]=$mask;
     ($pixmap, $mask) =create_from_xpm_d Gtk::Gdk::Pixmap( $Singleton->{window}->window, undef,  @yellow_xpm );
          $self->{pixmap}[1]=$pixmap;
          $self->{mask}[1]=$mask;
     ($pixmap, $mask) =create_from_xpm_d Gtk::Gdk::Pixmap( $Singleton->{window}->window, undef,  @red_xpm );
          $self->{pixmap}[2]=$pixmap;
          $self->{mask}[2]=$mask;
      }




sub create_clist {
	my (@titles, @text, $clist, $box1, $box2, $button, $separator, $policy_x,$policy_y, $size);

	( $policy_x, $policy_y,$size, @titles) = @_;

	my $i;
		my $scrolled_win = new Gtk::ScrolledWindow(undef, undef);
		$scrolled_win->set_policy($policy_x, $policy_y);

		$clist = new_with_titles Gtk::CList(@titles);
		$clist->set_row_height($size);

#		$clist->signal_connect('select_row', \&select_clist);
#		$clist->signal_connect('unselect_row', \&unselect_clist);      


		$clist->set_selection_mode('browse');
        for $i (0 ... $#titles){
		$clist->set_column_justification($i, 'center');
		$clist->set_column_min_width($i, 20);
		$clist->set_column_auto_resize($i, 1);
            }
		$clist->border_width(5);
		$scrolled_win->add($clist);
        return $scrolled_win, $clist;

    }

sub ReCreate{
             $Monitor::Singleton->RemoveRuns();
             DBServer::InitDBFileNew(undef,$Monitor::Singleton);
}
sub Update{
    if( $Monitor::Singleton->{updatesviadb}){
     $monitorUI::Singleton->{statusbar}->push(1," Retreiving DB Information");
    }
    else{
     $monitorUI::Singleton->{statusbar}->push(1," Retreiving Server Information");
    }
    while (Gtk->events_pending()){
           Gtk->main_iteration();
       }
     my ($monitor, $ok)=Monitor::Update();
     if(not $ok){
         $monitorUI::Singleton->{statusbar}->push(1," Unable to Update Servers");
         Warning();
         return;
     }else{
       if($Monitor::Singleton->{registered}==0){      
    if( $Monitor::Singleton->{updatesviadb}){
        $monitorUI::Singleton->{statusbar}->push(1," Connected to DB");
    }
    else{
        $monitorUI::Singleton->{statusbar}->push(1," Connected to Servers");
    }
       }
       else{
        $monitorUI::Singleton->{statusbar}->push(1," Connected and Registered to Servers");
       }
     }

{
     my $clist=$monitorUI::Singleton->{clist}->{Producer_ActiveHosts};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getactivehosts();     
     my $i;
     $monitorUI::Singleton->{notebook}[0]=0;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
         my $pmc=$#text-1;
     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[0]){
             $monitorUI::Singleton->{notebook}[0]=$text[$pmc+1];
         }
     }
     $clist->thaw();

}
{
     my $clist=$monitorUI::Singleton->{clist}->{Producer_ActiveClients};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getactiveclients("Producer");     
     my $i;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
         my $pmc=$#text-1;
     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[0]){
             $monitorUI::Singleton->{notebook}[0]=$text[$pmc+1];
         }
     }
     $clist->thaw();

}


{
     my $clist=$monitorUI::Singleton->{clist}->{Producer_Runs};
     $clist->freeze();   
     $clist->clear();
     $monitorUI::Singleton->{notebook}[1]=0;
     my @output=$monitor->getruns;
     my $i;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
         my $pmc=$#text-1;
     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[1]){
             $monitorUI::Singleton->{notebook}[1]=$text[$pmc+1];
         }
     }
     $clist->thaw();

}

{
     my $clist=$monitorUI::Singleton->{clist}->{Producer_Ntuples};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getntuples;
     my $i;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
         my $pmc=$#text-1;
     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[1]){
             $monitorUI::Singleton->{notebook}[1]=$text[$pmc+1];
         }
     }
     $clist->thaw();

}


{
     my $clist=$monitorUI::Singleton->{clist}->{Server_ActiveClients};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getactiveclients("Server");     
     my $i;
     $monitorUI::Singleton->{notebook}[2]=0;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
         my $pmc=$#text-1;
     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[2]){
             $monitorUI::Singleton->{notebook}[2]=$text[$pmc+1];
         }
     }
     $clist->thaw();

}

{
     my $clist=$monitorUI::Singleton->{clist}->{DBServer_ActiveClients};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getactiveclients("DBServer");     
     my $i;
     $monitorUI::Singleton->{notebook}[2]=0;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
         my $pmc=$#text-1;
     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[2]){
             $monitorUI::Singleton->{notebook}[2]=$text[$pmc+1];
         }
     }
     $clist->thaw();

}
#{
#     my $clist=$monitorUI::Singleton->{clist}->{Monitor_ActiveClients};
#     $clist->freeze();   
#     $clist->clear();
#     my @output=$monitor->getactiveclients("Monitor");     
#     my $i;
#     $monitorUI::Singleton->{notebook}[2]=0;
#     for $i (0 ... $#output){
#         my @text=@{$output[$i]};
#     $clist->append(@text);
#         my $pmc=$#text-1;
#     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
#         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[2]){
#             $monitorUI::Singleton->{notebook}[2]=$text[$pmc+1];
#         }
#     }
#     $clist->thaw();
#
#}

{
     my $clist=$monitorUI::Singleton->{clist}->{DiskUsage};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getdbok();     
     my $i;
     $monitorUI::Singleton->{notebook}[3]=0;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
     $clist->set_pixtext($i, 4, "", 5, $monitorUI::Singleton->{pixmap}[$text[4]], $monitorUI::Singleton->{mask}[$text[4]]);
         if($text[4]>$monitorUI::Singleton->{notebook}[3]){
             $monitorUI::Singleton->{notebook}[3]=$text[4];
         }
     }
     $clist->thaw();

}

     notebook_update_pages $monitorUI::Singleton->{notebookw}, 0, 3;
    warn "herehere ";
}

sub MonInfo{
    my ($message,$error,$timeout)=@_;
 my $s="\7";
 print $s;

my $window  =   new Gtk::Widget    "Gtk::Window",
                "GtkWidget::has_focus"            =>      1,
                "GtkWindow::type"                 =>      -toplevel,
                "GtkWindow::title"                =>      "Server Message",
                "GtkWindow::allow_grow"           =>      0,
                "GtkWindow::modal"                =>      1,
                "GtkWindow::window_position"             =>      -center,
                "GtkWindow::allow_shrink"         =>      0,
                "GtkContainer::border_width"      =>      10;

   
		$window->signal_connect( 'destroy', \&Gtk::Widget::destroyed, \$window );
                
                $window->realize();
                my $box = new Gtk::VBox(1,0);
                $window->add($box);
		my $label_box = new Gtk::HBox 0, 0;
                my ($book_closed, $book_closed_mask);
                if($error eq "Error" or $error eq "Severe"){
                 ($book_closed, $book_closed_mask) = create_from_xpm_d Gtk::Gdk::Pixmap( $window->window, undef, @red_xpm );
             }elsif($error eq "Warning"){
                 ($book_closed, $book_closed_mask) = create_from_xpm_d Gtk::Gdk::Pixmap( $window->window, undef, @yellow_xpm );
             }elsif($error eq "Info"){
                 ($book_closed, $book_closed_mask) = create_from_xpm_d Gtk::Gdk::Pixmap( $window->window, undef, @green_xpm );
             }
		my $pixwid = new Gtk::Pixmap $book_closed, $book_closed_mask;
		$label_box->pack_start($pixwid, 0, 1, 0);
		$pixwid->set_padding(3, 1);
		my $label = new Gtk::Label $message;
		$label_box->pack_start($label, 0, 1, 0);
                $box->pack_start($label_box,0,1,0);

        	my $button = new Gtk::Button( 'Close' );
		$button->signal_connect( 'clicked', sub { 
                   $window->destroy();} );
                $box->pack_start($button,0,1,0);
                my $popup=Gtk->timeout_add($timeout*1000, sub { 
                   $button->clicked } );

                show_all $window;
    while (Gtk->events_pending()){
           Gtk->main_iteration();
       }
}
sub MonDialog{
    my ($message,$error,$timeout)=@_;
 my $s="\7";
 print $s;

my $window  =   new Gtk::Widget    "Gtk::Window",
                "GtkWidget::has_focus"            =>      1,
                "GtkWindow::type"                 =>      -toplevel,
                "GtkWindow::title"                =>      "Server Query",
                "GtkWindow::allow_grow"           =>      0,
                "GtkWindow::modal"                =>      1,
                "GtkWindow::window_position"             =>      -center,
                "GtkWindow::allow_shrink"         =>      0,
                "GtkContainer::border_width"      =>      10;

   
		$window->signal_connect( 'destroy', \&Gtk::Widget::destroyed, \$window );
                
                $window->realize();
                my $box = new Gtk::VBox(1,0);
                $window->add($box);
		my $label_box = new Gtk::HBox 0, 0;
                my ($book_closed, $book_closed_mask);
                if($error eq "Error" or $error eq "Severe"){
                 ($book_closed, $book_closed_mask) = create_from_xpm_d Gtk::Gdk::Pixmap( $window->window, undef, @red_xpm );
             }elsif($error eq "Warning"){
                 ($book_closed, $book_closed_mask) = create_from_xpm_d Gtk::Gdk::Pixmap( $window->window, undef, @yellow_xpm );
             }elsif($error eq "Info"){
                 ($book_closed, $book_closed_mask) = create_from_xpm_d Gtk::Gdk::Pixmap( $window->window, undef, @green_xpm );
             }
		my $pixwid = new Gtk::Pixmap $book_closed, $book_closed_mask;
		$label_box->pack_start($pixwid, 0, 1, 0);
		$pixwid->set_padding(3, 1);
		my $label = new Gtk::Label $message;
		$label_box->pack_start($label, 0, 1, 0);
                $box->pack_start($label_box,0,1,0);


                my $hbox = new Gtk::HBox(1,10);
                $box->pack_start($hbox,0,1,0);
        	my $buttony = new Gtk::Button( 'Yes' );
		$buttony->signal_connect( 'clicked', sub { 
                  $monitorUI::Singleton->{queryanswer}=1;
                  $window->destroy } );
                $hbox->pack_start($buttony,1,1,0);
        	my $buttonn = new Gtk::Button( 'No' );
		$buttonn->signal_connect( 'clicked', sub { 
                  $monitorUI::Singleton->{queryanswer}=0;
                  $window->destroy } );
                $hbox->pack_start($buttonn,1,1,0);
                my $popup=Gtk->timeout_add($timeout*1000,sub { 
                   $buttony->clicked } );
                show_all $window;
    while (Gtk->events_pending()){
           Gtk->main_iteration();
       }
}
sub Warning(){
 my $s="\7";
 print $s;

my $window  =   new Gtk::Widget    "Gtk::Window",
                "GtkWidget::has_focus"            =>      1,
                "GtkWindow::type"                 =>      -toplevel,
                "GtkWindow::title"                =>      "Unable Connect Server",
                "GtkWindow::allow_grow"           =>      0,
                "GtkWindow::modal"                =>      1,
                "GtkWindow::window_position"             =>      -center,
                "GtkWindow::allow_shrink"         =>      0,
                "GtkContainer::border_width"      =>      10;

   
		$window->signal_connect( 'destroy', \&Gtk::Widget::destroyed, \$window );
                
                $window->realize();
                my $box = new Gtk::VBox(1,0);
                $window->add($box);
		my $label_box = new Gtk::HBox 0, 0;
                my ($book_closed, $book_closed_mask) = create_from_xpm_d Gtk::Gdk::Pixmap( $window->window, undef, @red_xpm );
		my $pixwid = new Gtk::Pixmap $book_closed, $book_closed_mask;
		$label_box->pack_start($pixwid, 0, 1, 0);
		$pixwid->set_padding(3, 1);
		my $label = new Gtk::Label "Unable To connect Server";
		$label_box->pack_start($label, 0, 1, 0);
                $box->pack_start($label_box,0,1,0);


        	my $button = new Gtk::Button( 'Close' );
		$button->signal_connect( 'clicked', sub { $window->destroy } );
                $box->pack_start($button,0,1,0);
#                Gtk->grab_add($window);
                show_all $window;
}

sub item_factory_cb {
	my ($widget, $action, @data) = @_;
        if($action==5){
           if($Monitor::Singleton->{ok}){
               if($Monitor::Singleton->{registered}){
                 $Monitor::Singleton->Exiting();
               }
           }
            exit();
        }elsif($action==1){
          Update();
         }elsif($action==2){
           $Monitor::Singleton->{registered}=$Monitor::Singleton->{registered}==0?1:0;
           if($Monitor::Singleton->{ok}){
               if($Monitor::Singleton->{registered}){
                 Monitor::SendId();
               }
               else{
                 $Monitor::Singleton->Exiting();
               }
           }
         }elsif($action==3){
           $Monitor::Singleton->{updatesviadb}=$Monitor::Singleton->{updatesviadb}==0?1:0;
         }elsif($action==6){
             $Monitor::Singleton->ForceCheck();
         }elsif($action==7){
             $Monitor::Singleton->ResetFailedRuns();
         }elsif($action==8){
             $Monitor::Singleton->ResetHosts();
         }elsif($action==9){
             $Monitor::Singleton->RemoveRuns();
         }elsif($action==10){
             $Monitor::Singleton->RestoreRuns1();
         }elsif($action==11){
             DBServer::InitDBFileNew(undef,$Monitor::Singleton);
         }elsif($action==12){
             $Monitor::Singleton->DeleteFailedDST();
         }elsif($action==13){
             $Monitor::Singleton->DeleteValidatedDst();
         }elsif($action==14){
             $Monitor::Singleton->FinishFailedRuns();
         }
    }

sub create_frame{
    my ($boxa,$self,$buffer,$policy_x, $policy_y, $size,@titles )= @_;
                        my $child = new Gtk::Frame( $buffer );
                        $child->border_width( 10 );
                        my $box = new Gtk::VBox( 1, 0 );
                        $box->border_width( 10 );
                        my ($scr, $clist)=create_clist($policy_x, $policy_y, $size,@titles);
                        $self->{clist}->{$buffer}=$clist;                          
                        $box->add($scr);
                        $child->add( $box );
		        $boxa->pack_start($child, 0,1,0);
}


sub update_list{

    my ($s1,$s2)=@_;
    my $list_item;
    $s1->clear_items(0,-1);
    if ($s2 eq "Producer"){
        $list_item = new Gtk::ListItem "Nominal Client";
        $s1->add($list_item);
        $list_item->show;
        $list_item->{name}="ProducerClient";
        $list_item->{clist}=undef;
        $list_item = new Gtk::ListItem "Active Client";
        $s1->add($list_item);
        $list_item->show;
        $list_item->{name}="ProducerActiveClient";
        $list_item->{clist}=undef;
        $list_item = new Gtk::ListItem "Nominal Host";
        $s1->add($list_item);
        $list_item->show;
        $list_item->{name}="ProducerHost";
        $list_item->{clist}=undef;
        $list_item = new Gtk::ListItem "Nominal Ntuple";
        $s1->add($list_item);
        $list_item->show;
        $list_item->{name}="Ntuple";
        $list_item->{clist}=undef;
        $list_item = new Gtk::ListItem "Produced Ntuple";
        $s1->add($list_item);
        $list_item->show;
        $list_item->{name}="PNtuple";
        $list_item->{clist}=undef;
        $list_item = new Gtk::ListItem "Run Table";
        $s1->add($list_item);
        $list_item->show;
        $list_item->{name}="Run";
        $list_item->{clist}=undef;
    }elsif($s2 eq "Server"){
        $list_item = new Gtk::ListItem "Nominal Client";
        $list_item->{name}="ServerClient";
        $list_item->{clist}=undef;
        $s1->add($list_item);
        $list_item->show;
        $list_item = new Gtk::ListItem "Nominal Host";
        $list_item->{name}="ServerHost";
        $list_item->{clist}=undef;
        $s1->add($list_item);
        $list_item->show;
        $list_item = new Gtk::ListItem "Nominal Killer";
        $list_item->{name}="Killer";
        $list_item->{clist}=undef;
        $s1->add($list_item);
        $list_item->show;
        $list_item = new Gtk::ListItem "set Environment";
        $list_item->{name}="setEnv";
        $list_item->{clist}=undef;
        $s1->add($list_item);
        $list_item->show;
    }
    $s1->show();

}

sub show_sample{
    my $name=shift;
    my $scr=shift;
    if(defined $scr->{clist}){
         $scr->remove($scr->{clist});
    }
    my @titles;
    if( $name eq "ServerClient"){
        $#titles=-1;
        @titles=(
          "Uid",
          "Type",
          "MaxClients",
          "CPU",
           "Memory",
             "ScriptPath",
            "LogPath",
            "Submit",
            "HostName",
           "LogInTheEnd",
             );                        
    }elsif( $name eq "ProducerClient"){
        $#titles=-1;
        @titles=(
          "Uid",
          "Type",
          "MaxClients",
          "CPU",
           "Memory",
             "ScriptPath",
            "LogPath",
            "Submit",
            "HostName",
           "LogInTheEnd",
             );                        

    }elsif( $name eq "ProducerActiveClient"){
        $#titles=-1;
        @titles=(
          "Uid",
          "HostName",
	  "Mips",
          "pid",
          "Status",
          "Type",
          "TimeOut",
             );                        

    }elsif( $name eq "ServerHost"){
        $#titles=-1;
        @titles=(
"HostName",
"Interface",
"OS",
"CPUNumber",
"Memory",
"Clock",
"Clients Allowed",
"Status",
                 );
    }elsif( $name eq "ProducerHost"){
        $#titles=-1;
        @titles=(
"HostName",
"Interface",
"OS",
"CPUNumber",
"Memory",
"Clock",
"Clients Allowed",
"Processes Failed",
"Status",
                 );
    }elsif( $name eq "Ntuple"){
        $#titles=-1;
        @titles=(
         "Uid",
        "HostName",
        "OutputDirPath",
        "RunMode",
        "UpdateFreq",
        "DSTType",
                 );
    }elsif( $name eq "PNtuple"){
        $#titles=-1;
        @titles=(
         "Run",
        "Time",
        "FirstEvent",
        "LastEvent",
        "Path",
        "Size",
        "Status",
        "Type",
                 );
    }elsif( $name eq "Run"){
        $#titles=-1;
        @titles=(
"Uid",
"CUid",
"Run",
"FirstEvent",
"LastEvent",
"Priority",
"FilePath",
"Status",
"History",
"StType",
"Fails",
                 );
    }elsif( $name eq "Killer"){
        $#titles=-1;
        @titles=(
          "Uid", 
          "Type",
          "MaxClients",
          "CPU",
           "Memory",
             "ScriptPath",
            "LogPath",
            "Submit",
            "HostName",
           "LogInTheEnd",
             );                        
    }elsif( $name eq "setEnv"){
        $#titles=-1;
        @titles=(
          "Environment Var", 
          "Path",
             );                        
    }
		my $clist = new_with_titles Gtk::CList(@titles);
		$clist->set_row_height(20);
                $scr->{clist}=$clist;
                $scr->{name}=$name;
                $scr->{columns}=$#titles; 
		$clist->signal_connect('select_row', \&select_clist);
		$clist->signal_connect('unselect_row', \&unselect_clist);      


		$clist->set_selection_mode('single');
        for my $i (0 ... $#titles){
		$clist->set_column_justification($i, 'center');
		$clist->set_column_min_width($i, 20);
		$clist->set_column_auto_resize($i, 1);
            }
		$clist->border_width(5);
		$scr->add($clist);
    $clist->show;
    $scr->show;
    $clist->freeze();
    $clist->clear();
    my @output=Monitor::getcontrolthings($name);
    for my $i (0 ... $#output){
        my @text=@{$output[$i]};
        $clist->append(@text);
        }
    $clist->thaw();
}               

sub unselect_clist{

    my($widget, $row, $column, $event) = @_;             
    #create text entry - modal;
    $widget->{row}=undef;
    $widget->{column}=undef;
}
sub select_clist{

    my($widget, $row, $column, $event) = @_;             
    #create text entry - modal;
    $widget->{row}=$row;
    $widget->{column}=$column;


my $window  =   new Gtk::Widget    "Gtk::Window",
                "GtkWidget::has_focus"            =>      1,
                "GtkWindow::type"                 =>      -toplevel,
                "GtkWindow::title"                =>      "Dialog",
                "GtkWindow::allow_grow"           =>      1,
                "GtkWindow::modal"                =>      1,
                "GtkWindow::window_position"             =>      -center,
                "GtkWindow::allow_shrink"         =>      0,
    "GtkContainer::border_width"      =>      10;

		$window->signal_connect( 'destroy', \&Gtk::Widget::destroyed, \$window );


    $window->realize();
    my $box = new Gtk::VBox(1,0);
    $window->add($box);
    my $label_box = new Gtk::HBox 0, 0;
    my $label = new Gtk::Label "Enter New Value:";
    $label_box->pack_start($label, 0, 1, 0);

    my $entry = new Gtk::Entry;
    my $text=$widget->get_text($row,$column);
    $entry->set_text($text);
    $entry->select_region(0, length($entry->get_text));
#    $entry->select_region(0, -1);
    $label_box->pack_start($entry, 0, 1, 0);                
    my $button = new Gtk::Button( 'OK' );
    $button->signal_connect( 'clicked', sub { 
        my $newtext=$entry->get_text();
        $widget->set_text($row,$column,$newtext);
        $window->destroy; 
 }
 );
    $box->pack_start($label_box,0,1,0);
    $box->pack_start($button,0,1,0);
    show_all $window;

    
}

sub ares{
    my ($widget,$action,$scr)=@_;
    if(defined $scr->{clist}){    
        if (defined      $scr->{clist}->{row}){
           my $name= $scr->{name};
            my @data=();
                for my $i( 0...$scr->{columns}){
                    push @data,$scr->{clist}->get_text($scr->{clist}->{row},$i);
                }
          Monitor::sendback($name,$action,$scr->{clist}->{row},@data);
        }
    }
}
