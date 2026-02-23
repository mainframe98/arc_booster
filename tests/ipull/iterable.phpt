--TEST--
arc_booster: ipull iterable
--EXTENSIONS--
arc_booster
--FILE--
<?php

function generate(bool $with_key) {
  foreach (range(1, 5) as $i) {
    if ($with_key) {
      yield $i => array('key' => "key $i", 'value' => "value $i");
    } else {
      yield array('key' => "key $i", 'value' => "value $i");
    }
  }
}

echo "Without key yielded:\n";

$res = ArcBooster\ipull(generate(false), 'value');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$res = ArcBooster\ipull(generate(false), 'value', 'key');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$res = ArcBooster\ipull(generate(false), null, 'key');
var_dump($res);

echo "\nWith key yielded:\n";

$res = ArcBooster\ipull(generate(true), 'value');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$res = ArcBooster\ipull(generate(true), 'value', 'key');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$res = ArcBooster\ipull(generate(true), null, 'key');
var_dump($res);

?>
--EXPECT--
Without key yielded:
'0': 'value 1'
'1': 'value 2'
'2': 'value 3'
'3': 'value 4'
'4': 'value 5'
'key 1': 'value 1'
'key 2': 'value 2'
'key 3': 'value 3'
'key 4': 'value 4'
'key 5': 'value 5'
array(5) {
  ["key 1"]=>
  array(2) {
    ["key"]=>
    string(5) "key 1"
    ["value"]=>
    string(7) "value 1"
  }
  ["key 2"]=>
  array(2) {
    ["key"]=>
    string(5) "key 2"
    ["value"]=>
    string(7) "value 2"
  }
  ["key 3"]=>
  array(2) {
    ["key"]=>
    string(5) "key 3"
    ["value"]=>
    string(7) "value 3"
  }
  ["key 4"]=>
  array(2) {
    ["key"]=>
    string(5) "key 4"
    ["value"]=>
    string(7) "value 4"
  }
  ["key 5"]=>
  array(2) {
    ["key"]=>
    string(5) "key 5"
    ["value"]=>
    string(7) "value 5"
  }
}

With key yielded:
'1': 'value 1'
'2': 'value 2'
'3': 'value 3'
'4': 'value 4'
'5': 'value 5'
'key 1': 'value 1'
'key 2': 'value 2'
'key 3': 'value 3'
'key 4': 'value 4'
'key 5': 'value 5'
array(5) {
  ["key 1"]=>
  array(2) {
    ["key"]=>
    string(5) "key 1"
    ["value"]=>
    string(7) "value 1"
  }
  ["key 2"]=>
  array(2) {
    ["key"]=>
    string(5) "key 2"
    ["value"]=>
    string(7) "value 2"
  }
  ["key 3"]=>
  array(2) {
    ["key"]=>
    string(5) "key 3"
    ["value"]=>
    string(7) "value 3"
  }
  ["key 4"]=>
  array(2) {
    ["key"]=>
    string(5) "key 4"
    ["value"]=>
    string(7) "value 4"
  }
  ["key 5"]=>
  array(2) {
    ["key"]=>
    string(5) "key 5"
    ["value"]=>
    string(7) "value 5"
  }
}
