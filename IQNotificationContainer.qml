/*
 *     This file is part of IQ Notifier.
 *
 * IQ Notifier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * IQ Notifier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with IQ Notifier.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.5
import QtQuick.Layouts 1.1
import IQNotifier 1.0

IQFancyContainer {
    id: root

    showDuration: IQThemes.notificationsTheme.showAnimationDuration
    dropDuration: IQThemes.notificationsTheme.dropAnimationDuration
    color: IQThemes.notificationsTheme.bgColor
    bgImageSource: IQThemes.notificationsTheme.bgImage

    signal buttonClicked(string button)

    property int referenceHeight: 0

    property alias appName: bar.text
    property alias title: titleText.text
    property alias body: bodyText.text
    property url iconUrl: ""
    property variant buttons: ""

    property alias expireTimeout: expirationBar.expireTimeout
    property bool expiration: false

    property int barHeight: IQThemes.notificationsTheme.barHeight ?
                                IQThemes.notificationsTheme.barHeight :
                                referenceHeight * barFactor
    property int expirationBarHeight: IQThemes.notificationsTheme.expirationBarHeight

    property int contentMargin: referenceHeight*spacingFactor*2
    property int fontPointSize: IQThemes.notificationsTheme.fontSize ?
                                    IQThemes.notificationsTheme.fontSize :
                                    referenceHeight * fontPointSizeFactor
    property int iconSize: IQThemes.notificationsTheme.iconSize ?
                                    IQThemes.notificationsTheme.iconSize :
                                    referenceHeight * iconFactor

    property real barFactor: 0.148;
    property real iconFactor: 0.3;
    property real spacingFactor: 0.04;
    property real buttonFactor: 0.13;
    property real fontPointSizeFactor: 0.045;

    IQNotificationBar {
        id: bar
        color: IQThemes.notificationsTheme.barBgColor
        textColor: IQThemes.notificationsTheme.barTextColor
        textFontSize: IQThemes.notificationsTheme.barFontSize ?
                          IQThemes.notificationsTheme.barFontSize :
                          height*0.4
        closeButtonImageSource: IQThemes.notificationsTheme.closeButtonImage
        elementsScale: IQThemes.notificationsTheme.closeButtonImageScale
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
        color: IQThemes.notificationsTheme.expirationBarColor
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
        sourceComponent: IQThemes.notificationsTheme.iconPosition ? iconComponent : undefined
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
            sourceComponent: !IQThemes.notificationsTheme.iconPosition ? iconComponent : undefined
            Layout.fillWidth: !buttonsLayout.visible
            Layout.fillHeight: Layout.fillWidth
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Text {
            id: titleText
            visible: text.length
            color: IQThemes.notificationsTheme.titleTextColor
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
            color: IQThemes.notificationsTheme.bodyTextColor
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
                    color: IQThemes.notificationsTheme.buttonBgColor
                    textColor: IQThemes.notificationsTheme.buttonTextColor
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: modelData.text
                    onClicked: buttonClicked(modelData.action)
                }
            }
        }
    }
}
