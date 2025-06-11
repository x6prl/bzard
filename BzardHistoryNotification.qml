import QtQuick

Rectangle {
    id: root
    width: parent.width
    height: 72
    opacity: mouseArea.containsMouse ? 0.9 : 1

    signal removeNotification(int index)

    property int sideMargin: height / 10
    property int topMargin: sideMargin / 2
    property alias appColor: appText.color
    property alias titleColor: titleText.color
    property alias bodyColor: bodyText.color

    property alias appSize: appText.font.pointSize
    property alias titleSize: titleText.font.pointSize
    property alias bodySize: bodyText.font.pointSize

    Image {
        id: icon
        width: height
        fillMode: Image.PreserveAspectFit
        source: iconUrl
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
    }

    Text {
        id: appText
        text: application
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
        anchors.left: parent.left
        anchors.rightMargin: sideMargin
        anchors.leftMargin: sideMargin
        anchors.bottomMargin: topMargin
        anchors.bottom: parent.bottom
        anchors.right: parent.right
    }

    Text {
        id: titleText
        text: title
        anchors.right: parent.right
        wrapMode: Text.WordWrap
        anchors.leftMargin: sideMargin
        anchors.rightMargin: sideMargin
        anchors.topMargin: topMargin
        anchors.top: parent.top
        anchors.left: icon.right
    }
    Text {
        id: bodyText
        text: body
        anchors.top: titleText.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: sideMargin
        anchors.rightMargin: sideMargin
        anchors.topMargin: topMargin
        anchors.left: icon.right
        wrapMode: Text.WordWrap
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: removeNotification(index)
    }

    ListView.onAdd: ParallelAnimation {
        NumberAnimation { target: root; property: "scale"; from: 0; to: 1; duration: 250; easing.type: Easing.InOutQuad }
        NumberAnimation { target: root; property: "height"; from: 0; to: height; duration: 250; easing.type: Easing.InOutQuad }
    }

    ListView.onRemove: SequentialAnimation {
        PropertyAction { target: root; property: "ListView.delayRemove"; value: true }
        ParallelAnimation{
            NumberAnimation { target: root; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
            NumberAnimation { target: root; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
        }

        // Make sure delayRemove is set back to false so that the item can be destroyed
        PropertyAction { target: root; property: "ListView.delayRemove"; value: false }
    }
}
