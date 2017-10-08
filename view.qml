import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

Rectangle {
    id: root

    width: 900; height: 400

    Component
    {
        id: columnComponent
        TableViewColumn{width: 120}
    }

    GridLayout {
        id: coloumnGrid
        anchors.fill: parent
        rows : 2
        columns : 1

        RowLayout
        {
            id : rowLayout
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.row: 1;
            TableView {
                id: view
                Layout.fillWidth: true
                Layout.fillHeight: true
                resources:
                {
                    var widgets = []
                    if (model)
                    {
                        // generate table coloumns according to the given dataheaders of the model
                        var roleList = model.dataHeaders
                        for(var i=0; i<roleList.length; i++)
                        {
                            var role  = roleList[i]
                            widgets.push(columnComponent.createObject(view, { "role": role, "title": role}))
                        }
                    }
                    return widgets
                }

                model: cpuInfo
            }
        }

        RowLayout
        {
            id : rowLayout2
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight

            Layout.row: 2

            CheckBox {
                id: myCheck
                text: qsTr("Track")
                Layout.alignment: Qt.AlignRight
                onClicked: cpuInfo.track = checked
                Component.onCompleted: checked = cpuInfo.track
            }
        }
    }
}
