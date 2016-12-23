use Env qw[$HOME];

$config{'separator'}=";";
$config{'skyliteHostPort'}="localhost:3121";
$config{skyliteURL} = "http://$config{skyliteHostPort}/skylitedcpp";
$config{'hist_file'}="$HOME/.config/skylitedc++++/skylitedcpp_xcli.hist";
$config{'hist_max'}=500;
$config{'prompt'}="# ";

1;
