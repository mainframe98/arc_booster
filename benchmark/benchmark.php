<?php

function bench_array_mergev_packed($n) {
  $mergev_array = array(
    range(1, 15),
    range(1, 15),
    range(1, 15),
    range(1, 15),
    range(1, 15),
    range(1, 15),
    range(1, 15),
    range(1, 15),
    range(1, 15),
  );

  for ($i = 0; $i < $n; $i++) {
    \ArcBooster\array_mergev($mergev_array);
  }
}

function bench_array_mergev_hash($n) {
  $keys = array_map(static fn ($i) => md5((string)$i), range(2, 16));
  $mergev_array = array(
    array_fill_keys($keys, true),
    array_fill_keys($keys, true),
    array_fill_keys($keys, true),
    array_fill_keys($keys, true),
    array_fill_keys($keys, true),
    array_fill_keys($keys, true),
    array_fill_keys($keys, true),
    array_fill_keys($keys, true),
    array_fill_keys($keys, true),
  );

  for ($i = 0; $i < $n; $i++) {
    \ArcBooster\array_mergev($mergev_array);
  }
}

function bench_array_mergev_mixed($n) {
  $keys = array_map(static fn ($i) => md5((string)$i), range(2, 16));
  $mergev_array = array(
    range(1, 15),
    range(15, 29),
    array_fill_keys($keys, true),
    array_fill_keys(range(72, 87), true),
    array_fill_keys($keys, array()),
    range(1, 15),
    range(1, 15),
    array_fill_keys(range(2, 16), 'test'),
    range(1, 15),
    array_fill_keys(range(2, 16), true),
  );

  for ($i = 0; $i < $n; $i++) {
    \ArcBooster\array_mergev($mergev_array);
  }
}

function gethrtime() {
  $hrtime = hrtime();
  return (($hrtime[0] * 1000000000 + $hrtime[1]) / 1000000000);
}

function start_test() {
  ob_start();
  return gethrtime();
}

function end_test($start, $name) {
  global $total;
  $end = gethrtime();
  ob_end_clean();
  $last_time = $end - $start;
  $total += $last_time;
  $num = number_format($last_time, 3);
  $pad = str_repeat(" ", 24 - strlen($name) - strlen($num));
  echo "$name$pad$num\n";
  ob_start();
  return gethrtime();
}

function total() {
  global $total;
  $pad = str_repeat("-", 24);
  echo $pad."\n";
  $num = number_format($total, 3);
  $pad = str_repeat(" ", 24 - strlen("Total") - strlen($num));
  echo "Total$pad$num\n";
}

const N = 5000000;

$t = start_test();
bench_array_mergev_packed(N);
$t = end_test($t, 'array_mergev packed');
bench_array_mergev_hash(N);
$t = end_test($t, 'array_mergev hash');
bench_array_mergev_mixed(N);
$t = end_test($t, 'array_mergev mixed');
total();
