<?php

namespace GosaSystems\admin\systems;

bindtextdomain("systems", dirname(dirname(__FILE__)) . "/locale/compiled");

function __($GETTEXT) {
    return dgettext("systems", $GETTEXT);
}
