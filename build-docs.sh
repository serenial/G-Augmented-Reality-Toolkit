#!/usr/bin/env sh

cd "$(dirname "$0")"

RUN_SETUP=false;
RUN_UI_BUILD=false;

while [[ $# -gt 0 ]]; do
  case $1 in
    -a|--all)
      RUN_SETUP=true;
      RUN_UI_BUILD=true;
      shift # past argument
      ;;
    -u|--ui)
      RUN_UI_BUILD=true;
      shift # past argument
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

if [ "$RUN_SETUP" = true ] ; then
    npm install && npm --prefix docs-UI install
fi

if [ "$RUN_UI_BUILD" = true ] ; then
    docs-UI/node_modules/gulp/bin/gulp.js --gulpfile docs-UI/gulpfile.js bundle
fi

npx antora antora-playbook.yml 