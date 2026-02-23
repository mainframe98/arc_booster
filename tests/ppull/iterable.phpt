--TEST--
arc_booster: ppull iterable
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Getter {
  public function __construct(public $key, public $value) {}
}

function generate(bool $with_key) {
  foreach (range(1, 5) as $i) {
    if ($with_key) {
      yield $i => new Getter("key $i", "value $i");
    } else {
      yield new Getter("key $i", "value $i");
    }
  }
}

echo "Without key yielded:\n";

$res = ArcBooster\ppull(generate(false), 'value');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$res = ArcBooster\ppull(generate(false), 'value', 'key');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$res = ArcBooster\ppull(generate(false), null, 'key');
var_dump($res);

echo "\nWith key yielded:\n";

$res = ArcBooster\ppull(generate(true), 'value');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$res = ArcBooster\ppull(generate(true), 'value', 'key');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$res = ArcBooster\ppull(generate(true), null, 'key');
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
  object(Getter)#3 (2) {
    ["key"]=>
    string(5) "key 1"
    ["value"]=>
    string(7) "value 1"
  }
  ["key 2"]=>
  object(Getter)#4 (2) {
    ["key"]=>
    string(5) "key 2"
    ["value"]=>
    string(7) "value 2"
  }
  ["key 3"]=>
  object(Getter)#5 (2) {
    ["key"]=>
    string(5) "key 3"
    ["value"]=>
    string(7) "value 3"
  }
  ["key 4"]=>
  object(Getter)#6 (2) {
    ["key"]=>
    string(5) "key 4"
    ["value"]=>
    string(7) "value 4"
  }
  ["key 5"]=>
  object(Getter)#7 (2) {
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
  object(Getter)#6 (2) {
    ["key"]=>
    string(5) "key 1"
    ["value"]=>
    string(7) "value 1"
  }
  ["key 2"]=>
  object(Getter)#4 (2) {
    ["key"]=>
    string(5) "key 2"
    ["value"]=>
    string(7) "value 2"
  }
  ["key 3"]=>
  object(Getter)#3 (2) {
    ["key"]=>
    string(5) "key 3"
    ["value"]=>
    string(7) "value 3"
  }
  ["key 4"]=>
  object(Getter)#1 (2) {
    ["key"]=>
    string(5) "key 4"
    ["value"]=>
    string(7) "value 4"
  }
  ["key 5"]=>
  object(Getter)#8 (2) {
    ["key"]=>
    string(5) "key 5"
    ["value"]=>
    string(7) "value 5"
  }
}
