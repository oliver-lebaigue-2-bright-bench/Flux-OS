#!/bin/bash
# Flux-OS Theme Installation Script

set -e

echo "Installing Flux-OS Nothing (R) Theme..."

# Create theme directories
echo "Creating theme directories..."
mkdir -p /usr/share/themes/Flux-Nothing/{gtk-4.0,qt,web}
mkdir -p /usr/share/icons/Flux-Nothing/{16x16,24x24,32x32,48x48,64x64}

# Copy GTK theme
echo "Installing GTK 4 theme..."
cp recipes/gui/themes/gtk/Flux-Nothing/gtk.css /usr/share/themes/Flux-Nothing/gtk-4.0/

# Copy Qt theme
echo "Installing Qt theme..."
cp recipes/gui/themes/qt/Flux-Nothing/theme.xml /usr/share/themes/Flux-Nothing/qt/

# Copy web theme
echo "Installing web theme..."
cp recipes/gui/themes/web/flux-nothing.css /usr/share/themes/Flux-Nothing/web/

# Copy Orbital theme
echo "Installing Orbital theme..."
cp recipes/gui/themes/orbital/flux-nothing.toml /usr/share/themes/Flux-Nothing/

# Copy icons/glyphs
echo "Installing icon glyphs..."
cp -r recipes/gui/themes/glyphs/* /usr/share/icons/Flux-Nothing/

# Update icon cache
if command -v update-icon-caches &> /dev/null; then
    echo "Updating icon cache..."
    update-icon-caches /usr/share/icons/Flux-Nothing
fi

# Create symlink for gtk-4.0 settings.ini if needed
if [ ! -d ~/.config/gtk-4.0 ]; then
    mkdir -p ~/.config/gtk-4.0
fi

echo "Theme installed successfully!"
echo ""
echo "To use the theme:"
echo "  GTK apps: GTK_THEME=Flux-Nothing"
echo "  Qt apps: QT_STYLE_OVERRIDE=Flux-Nothing"
echo "  Web apps: Include flux-nothing.css in your HTML"
echo ""
echo "Flux-OS Theme is now active. Enjoy the minimalist design!"
