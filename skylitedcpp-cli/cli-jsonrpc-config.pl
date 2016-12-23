use Env qw[$HOME];

$config{'debug'}=0;
$config{'separator'}=";";
$config{'skyliteHostPort'}="localhost:3121";
$config{'skyliteURL'} = "http://$config{skyliteHostPort}";
$config{'hist_file'}="$HOME/.config/skylitedc++++/skylitedcpp_jcli.hist";
$config{'hist_max'}=500;
$config{'jsonrpc'}="2.0";
$config{'prompt'}="# ";

1;
