/*
 *     This file is part of bzard.
 *
 * bzard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * bzard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bzard.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick
import QtQuick.Layouts
import bzard 1.0

BzardFancyContainer {
    id: root

    showDuration: BzardThemes.notificationsTheme.showAnimationDuration
    dropDuration: BzardThemes.notificationsTheme.dropAnimationDuration
    color: BzardThemes.notificationsTheme.bgColor
    bgImageSource: BzardThemes.notificationsTheme.bgImage

    signal buttonClicked(string button)

    property int referenceHeight: 0

    property alias appName: bar.text
    property alias title: titleText.text
    property alias body: bodyText.text
    property url iconUrl: ""
    property variant buttons: ""

    property alias expireTimeout: expirationBar.expireTimeout
    property bool expiration: false

    property int barHeight: BzardThemes.notificationsTheme.barHeight ?
                                BzardThemes.notificationsTheme.barHeight :
                                referenceHeight * barFactor
    property int expirationBarHeight: BzardThemes.notificationsTheme.expirationBarHeight

    property int contentMargin: referenceHeight*spacingFactor*2
    property int fontPointSize: BzardThemes.notificationsTheme.fontSize ?
                                    BzardThemes.notificationsTheme.fontSize :
                                    referenceHeight * fontPointSizeFactor
    property int iconSize: BzardThemes.notificationsTheme.iconSize ?
                                    BzardThemes.notificationsTheme.iconSize :
                                    referenceHeight * iconFactor

    property real barFactor: 0.148;
    property real iconFactor: 0.3;
    property real spacingFactor: 0.04;
    property real buttonFactor: 0.13;
    property real fontPointSizeFactor: 0.045;

    IQNotificationBar {
        id: bar
        color: BzardThemes.notificationsTheme.barBgColor
        textColor: BzardThemes.notificationsTheme.barTextColor
        textFontSize: BzardThemes.notificationsTheme.barFontSize ?
                          BzardThemes.notificationsTheme.barFontSize :
                          height*0.4
        closeButtonImageSource: BzardThemes.notificationsTheme.closeButtonImage
        elementsScale: BzardThemes.notificationsTheme.closeButtonImageScale
        height: barHeight;
        visible: height
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        onCloseClicked: root.closeClicked()
    }

    IQExpirationBar {
        id: expirationBar
        anchors.top: bar.bottom
        anchors.left: parent.left
        color: BzardThemes.notificationsTheme.expirationBarColor
        height: expirationBarHeight
        // Crutch to run animation after object created
        runnig: expiration && root.height == root.referenceHeight && root.height > 0
    }

    Component {
        id: iconComponent
        Image {
            id: icon
            source: iconUrl
            sourceSize.width: iconSize
            sourceSize.height: iconSize
            fillMode: Image.PreserveAspectFit
            visible: source.toString().length
        }
    }

    Loader {
        id: iconAtLeftSideLoader
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: bar.bottom
        anchors.leftMargin: contentMargin
        visible: sourceComponent != undefined
        sourceComponent: BzardThemes.notificationsTheme.iconPosition ? iconComponent : undefined
    }

    ColumnLayout {
        id: column
        spacing: referenceHeight * spacingFactor
        anchors.rightMargin: contentMargin
        anchors.leftMargin: contentMargin
        anchors.bottomMargin: contentMargin
        anchors.topMargin: contentMargin
        anchors.top: bar.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: iconAtLeftSideLoader.right

        Loader {
            visible: sourceComponent != undefined
            sourceComponent: !BzardThemes.notificationsTheme.iconPosition ? iconComponent : undefined
            Layout.fillWidth: !buttonsLayout.visible
            Layout.fillHeight: Layout.fillWidth
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Text {
            id: titleText
            visible: text.length
            color: BzardThemes.notificationsTheme.titleTextColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: root.fontPointSize
            font.weight: Font.Medium
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
        Text {
            id: bodyText
            color: BzardThemes.notificationsTheme.bodyTextColor
            visible: text.length
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.pointSize: titleText.font.pointSize
            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        RowLayout {
            id: buttonsLayout
            visible: buttons.length
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: false
            Repeater {
                model: buttons
                IQButton {
                    height: referenceHeight * buttonFactor
                    color: BzardThemes.notificationsTheme.buttonBgColor
                    textColor: BzardThemes.notificationsTheme.buttonTextColor
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: modelData.text
                    onClicked: buttonClicked(modelData.action)
                }
            }
        }
    }
}
