#! /bin/bash

# Author:  Boris Pek <tehnick-8@mail.ru>
# License: Public Domain
# Created: 2011-11-26
# Updated: 2013-06-26
# Version: N/A

if [[ ${0} =~ ^/.+$ ]]; then
    export CUR_DIR="$(dirname ${0})"
else
    export CUR_DIR="${PWD}/$(dirname ${0})"
fi

export MAIN_DIR="${CUR_DIR}/.."

cd "${CUR_DIR}" || exit 1

case "${1}" in
"up")

    git pull --all || exit 1

;;
"cm")

    git commit -a -m 'Translations were updated from Transifex.' || exit 1

;;
"make")

    if [ -d "${CUR_DIR}/builddir" ]; then
        cd "${CUR_DIR}/builddir" || exit 1
        make translations_qt mo-update || exit 1
        
    else
        mkdir -p builddir && cd builddir || exit 1
        cmake -DUSE_QT=ON -DUSE_GTK=ON .. || exit 1
        "${0}" make || exit 1
    fi

;;
"tr")

    # Test Internet connection:
    host transifex.net > /dev/null || exit 1

    git status || exit 1

    LANG_DIR="${MAIN_DIR}/skylitedcpp_transifex"

    cd "${LANG_DIR}" || exit 1
    tx pull -a -s || exit 1

    cd "${LANG_DIR}/translations/skylitedcpp.dcpp" || exit 1
    cp *.po "${CUR_DIR}/dcpp/po/" || exit 1

    cd "${LANG_DIR}/translations/skylitedcpp.skylitedcpp-gtk" || exit 1
    cp *.po "${CUR_DIR}/skylitedcpp-gtk/po/" || exit 1

    cd "${LANG_DIR}/translations/skylitedcpp.skylitedcpp-qt" || exit 1
    cp *.ts "${CUR_DIR}/skylitedcpp-qt/translations/" || exit 1

    cd "${CUR_DIR}" || exit 1
    git status || exit 1

;;
"tr_up")

    cd "${CUR_DIR}" || exit 1

    export QT_SELECT=qt4
    lupdate skylitedcpp-qt/translations.pro || exit 1

    cd builddir && make pot-update || exit 1
    cp -fa dcpp/po/libskylitedcpp.pot dcpp/po/en.po
    cp -fa skylitedcpp-gtk/po/skylitedcpp-gtk.pot skylitedcpp-gtk/po/en.po

    git status || exit 1

;;
"tr_cl")

    cd "${CUR_DIR}" || exit 1

    export QT_SELECT=qt4
    lupdate -verbose -no-obsolete skylitedcpp-qt/translations.pro || exit 1

    cd builddir && make pot-update || exit 1
    cp -fa dcpp/po/libskylitedcpp.pot dcpp/po/en.po
    cp -fa skylitedcpp-gtk/po/skylitedcpp-gtk.pot skylitedcpp-gtk/po/en.po

    git status || exit 1

;;
"tr_push")

    LANG_DIR="${MAIN_DIR}/skylitedcpp_transifex"
    cd "${LANG_DIR}/translations" || exit 1

    cp "${CUR_DIR}"/dcpp/po/*.po skylitedcpp.dcpp/ || exit 1
    cp "${CUR_DIR}"/skylitedcpp-gtk/po/*.po skylitedcpp.skylitedcpp-gtk/ || exit 1
    cp "${CUR_DIR}"/skylitedcpp-qt/translations/*.ts skylitedcpp.skylitedcpp-qt/ || exit 1

    cd "${LANG_DIR}" || exit 1
    if [ -z "${2}" ]; then
        echo "<arg> is not specified!" ; exit 1
    elif [ "${2}" = "src" ] ; then
        tx push -s || exit 1
    elif [ "${2}" = "all" ] ; then
        tx push -s -t || exit 1
    else
        tx push -t -l ${2} || exit 1
    fi

;;
"tr_co")

    if [ -d "${MAIN_DIR}/skylitedcpp_transifex" ]; then
        echo "${MAIN_DIR}/skylitedcpp_transifex"
        echo "directory already exists!"
    else
        echo "Creating ${MAIN_DIR}/skylitedcpp_transifex"
        mkdir -p "${MAIN_DIR}/skylitedcpp_transifex/.tx"
        cp "transifex.config" "${MAIN_DIR}/skylitedcpp_transifex/.tx/config" || exit 1
        cd "${MAIN_DIR}/skylitedcpp_transifex" || exit 1
        tx pull -a -s || exit 1
    fi

;;
*)

    echo "Usage:"
    echo "  up cm make"
    echo "  tr tr_up tr_cl tr_co"
    echo "  tr_push <arg> (arg: src, all or language)"
    echo ;
    echo "Examples:"
    echo "  ./update-translations.sh tr_push src"
    echo "  ./update-translations.sh tr_push all"
    echo "  ./update-translations.sh tr_push ru"

;;
esac

exit 0
