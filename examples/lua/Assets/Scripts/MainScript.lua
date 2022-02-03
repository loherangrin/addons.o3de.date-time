-- Copyright 2022 Matteo Grasso
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.


local MainScript =
{
	Properties =
	{
		MaxAlarmNotificationTime = 2.0
	}
}

function MainScript:OnActivate()
	self.isNotificationCloseDate = false
	self.notificationCloseDate = Date()
	self.notificationCloseTime = Time()

	self.canvasRefNotificationsHandler = UiCanvasRefNotificationBus.Connect(self, self.entityId)

	local mainCanvasId = UiCanvasRefBus.Event.GetCanvas(self.entityId)
	self:FindAllUiElements(mainCanvasId)

	self:AskForCurrentDate()
	self:AskForCurrentTime()

	-- -------------------------------------
	-- [INSTRUCTIONS]: #1
	self.alarmNotificationsHandler = AlarmNotificationBus.Connect(self, self.entityId)
	self.dateNotificationsHandler = DateNotificationBus.Connect(self)
	self.timeNotificationsHandler = TimeNotificationBus.Connect(self)
	-- -------------------------------------
end

function MainScript:OnDeactivate()
	-- -------------------------------------
	-- [INSTRUCTIONS]: #2
	self.timeNotificationsHandler = nil
	self.dateNotificationsHandler = nil
	self.alarmNotificationsHandler = nil
	-- -------------------------------------

	self.canvasRefNotificationsHandler = nil
end

function MainScript:OnCanvasRefChanged(uiCanvasRefEntity, uiCanvasEntity)
	self:FindAllUiElements(uiCanvasEntity)
end

-- -------------------------------------
-- [INSTRUCTIONS]: #3

function MainScript:OnAlarmBell(date, time)
	local notificationText = self:FormatMessageWithDateTime("Received a new notification", date, time)
	self:EditTextUiElement(self.currentAlarmTextId, notificationText)

	self:ShowNotification(notificationText, date, time)
end

function MainScript:OnDateDayChanged(nowDate)
	self:EditTextUiElement(self.dateYearTextId, tostring(nowDate.Year))
	self:EditTextUiElement(self.dateMonthTextId, nowDate.MonthName)
	self:EditTextUiElement(self.dateDayTextId, tostring(nowDate.DayNumber))

	if self.notificationCloseDate:IsValid() and (notificationCloseDate == nowDate) then
		self.isNotificationCloseDate = true
	end
end

function MainScript:OnTimeSecondChanged(nowTime)
	self:EditTextUiElement(self.timeHoursTextId, tostring(nowTime.Hours))
	self:EditTextUiElement(self.timeMinutesTextId, tostring(nowTime.Minutes))
	self:EditTextUiElement(self.timeSecondsTextId, tostring(nowTime.Seconds))

	self:CloseNotificationIfExpired(nowTime)

	self:AskForNextAlarm()
end

-- -------------------------------------

function MainScript:FormatMessageWithDateTime(message, date, time)
	local output = message

	if date:IsValid() then
		output = output .. " on " .. tostring(date)
	end

	if time:IsValid() then
		output = output .. " at " .. tostring(time)
	end

	return output
end

function MainScript:AskForCurrentDate()
	local nowDate = DateRequestBus.Broadcast.GetCurrentDate()
	self:OnDateDayChanged(nowDate)
end

function MainScript:AskForCurrentTime()
	local nowTime = TimeRequestBus.Broadcast.GetCurrentTime()
	self:OnTimeSecondChanged(nowTime)
end

function MainScript:AskForNextAlarm()
	local result = AlarmRequestBus.Broadcast.GetWhenNextAlarm()
	local nextDate = result.first
	local nextTime = result.second

	local nextAlarmText = ""
	if not nextTime:IsValid() then
		nextAlarmText = "No alarm is left"
	else
		nextAlarmText = self:FormatMessageWithDateTime("Next alarm will be", nextDate, nextTime)
	end

	self:EditTextUiElement(self.nextAlarmTextId, nextAlarmText)
end

function MainScript:ShowNotification(message, nowDate, nowTime)
	self:ShowUiElement(self.notificationPanelId)

	local notificationTimeout = TimeRequestBus.Broadcast.ConvertToSceneTime(self.Properties.MaxAlarmNotificationTime)

	local remainder = 0.0
	if nowDate:IsValid() then
		self.notificationCloseDate = nowDate

		local result = self.notificationCloseDate:MoveBySeconds(notificationTimeout)
		remainder = result.first
		local dateChange = result.second

		self.isNotificationCloseDate = (dateChange == DateChange_None)
	else
		remainder = notificationTimeout
		self.isNotificationCloseDate = true
	end

	self.notificationCloseTime = nowTime
	self.notificationCloseTime:MoveBySeconds(remainder)
end

function MainScript:CloseNotificationIfExpired(nowTime)
	if not self.notificationCloseTime:IsValid() then
		return
	end

	if self.isNotificationCloseDate and (nowTime >= self.notificationCloseTime) then
		self:HideUiElement(self.notificationPanelId)

		self.notificationCloseDate = Date()
		self.notificationCloseTime = Time()
	end
end

function MainScript:FindAllUiElements(mainCanvasId)
	if (mainCanvasId == self.mainCanvasId) or not mainCanvasId:IsValid() then
		return
	end

	self.mainCanvasId = mainCanvasId

	self.dateYearTextId = self:FindUiElement("YearFieldValue")
	self.dateMonthTextId = self:FindUiElement("MonthFieldValue")
	self.dateDayTextId = self:FindUiElement("DayFieldValue")

	self.timeHoursTextId = self:FindUiElement("HoursFieldValue")
	self.timeMinutesTextId = self:FindUiElement("MinutesFieldValue")
	self.timeSecondsTextId = self:FindUiElement("SecondsFieldValue")

	self.notificationPanelId = self:FindUiElement("NotificationPanel")

	self.currentAlarmTextId = self:FindUiElement("CurrentAlarmValue")
	self.nextAlarmTextId = self:FindUiElement("NextAlarmValue")
end

function MainScript:FindUiElement(elementName)
	local elementId = UiCanvasBus.Event.FindElementByName(self.mainCanvasId, elementName)

	return elementId
end

function MainScript:EditTextUiElement(textElementId, textValue)
	UiTextBus.Event.SetText(textElementId, textValue)
end

function MainScript:ShowUiElement(elementId)
	UiElementBus.Event.SetIsEnabled(elementId, true)
end

function MainScript:HideUiElement(elementId)
	UiElementBus.Event.SetIsEnabled(elementId, false)
end

return MainScript
