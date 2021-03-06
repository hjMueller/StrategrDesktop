#!/bin/bash
build_path=$1
app_name="Strategr"
codesign_id=$(cat "$build_path/../scripts/cert_id.txt")
dmg_path="$build_path/$app_name.dmg"
dmg_template_path="$build_path/../deployment/package.dmg"
dmg_source_path="./DMGContainer"

rm -rf "$dmg_path" || true

echo "Deploying macOS application"

~/Qt/5.13.0/clang_64/bin/macdeployqt "$build_path/$app_name.app" \
				-codesign="$codesign_id"

echo "App bundle created"

echo "Creating .dmg"

mkdir "$dmg_source_path"
cp -r "$build_path/$app_name.app" "$dmg_source_path/$app_name.app"

hdiutil create "$dmg_template_path" -ov \
				-volname "$app_name" \
				-fs HFS+ \
				-srcfolder "$dmg_source_path"

hdiutil convert "$dmg_template_path" -format UDZO -o "$dmg_path"

codesign -s "$codesign_id" \
				--keychain "~/Library/Keychains/login.keychain" "$dmg_path"

rm -rf "$dmg_source_path"

echo ".dmg created"

echo "Creating updater archive"
cd "$build_path" || exit
zip -r "$build_path/macOS_update.zip" "Strategr.app"
echo "Updater archive created"

