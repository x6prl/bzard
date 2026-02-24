/*
 * Minimal LayerShellQt interface declarations for build environments where
 * headers are not installed but the shared library is available.
 */

#pragma once

#include <QMargins>
#include <QObject>
#include <QSize>
#include <QString>
#include <QWindow>

namespace LayerShellQt {

class Shell {
  public:
	static void useLayerShell();
};

class Window : public QObject {
  public:
	enum Anchor {
		AnchorNone = 0,
		AnchorTop = 1,
		AnchorBottom = 2,
		AnchorLeft = 4,
		AnchorRight = 8,
	};
	Q_DECLARE_FLAGS(Anchors, Anchor)

	enum Layer {
		LayerBackground = 0,
		LayerBottom = 1,
		LayerTop = 2,
		LayerOverlay = 3,
	};

	enum KeyboardInteractivity {
		KeyboardInteractivityNone = 0,
		KeyboardInteractivityExclusive = 1,
		KeyboardInteractivityOnDemand = 2,
	};

	static Window *get(QWindow *window);

	void setAnchors(Anchors anchors);
	void setMargins(const QMargins &margins);
	void setDesiredSize(const QSize &size);
	void setExclusiveZone(int32_t zone);
	void setLayer(Layer layer);
	void setKeyboardInteractivity(KeyboardInteractivity interactivity);
	void setScope(const QString &scope);
};

} // namespace LayerShellQt

Q_DECLARE_OPERATORS_FOR_FLAGS(LayerShellQt::Window::Anchors)
