--TEST--
arc_booster: AST manipulations for call_user_func/call_user_func_array
--EXTENSIONS--
arc_booster
--FILE--
<?php

namespace {
  function phutil_is_natural_list(array $a) {}
}

namespace Test {
  \call_user_func('phutil_is_natural_list', array());
  \call_user_func('\phutil_is_natural_list', array());
  \call_user_func('\\phutil_is_natural_list', array());
  \call_user_func_array('phutil_is_natural_list', array(array()));
  \call_user_func_array('\phutil_is_natural_list', array(array()));
  \call_user_func_array('\\phutil_is_natural_list', array(array()));
}

namespace {
  call_user_func('phutil_is_natural_list', array());
  call_user_func('\phutil_is_natural_list', array());
  call_user_func('\\phutil_is_natural_list', array());
  call_user_func_array('phutil_is_natural_list', array(array()));
  call_user_func_array('\phutil_is_natural_list', array(array()));
  call_user_func_array('\\phutil_is_natural_list', array(array()));

  echo ArcBooster\render_ast(file_get_contents(__FILE__));
}

?>
--EXPECT--

namespace {
    function phutil_is_natural_list(array $a) {
    }

}

namespace Test {
    \call_user_func('phutil_is_natural_list', []);
    \call_user_func('\\array_is_list', []);
    \call_user_func('\\array_is_list', []);
    \call_user_func_array('phutil_is_natural_list', [[]]);
    \call_user_func_array('\\array_is_list', [[]]);
    \call_user_func_array('\\array_is_list', [[]]);
}

namespace {
    call_user_func('\\array_is_list', []);
    call_user_func('\\array_is_list', []);
    call_user_func('\\array_is_list', []);
    call_user_func_array('\\array_is_list', [[]]);
    call_user_func_array('\\array_is_list', [[]]);
    call_user_func_array('\\array_is_list', [[]]);
    echo ArcBooster\render_ast(file_get_contents(__FILE__));
}
