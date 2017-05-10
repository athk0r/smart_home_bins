/**
 * Copyright (c) 2014-2015 openHAB UG (haftungsbeschraenkt) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */
package org.openhab.binding.sdds.handler;

import org.eclipse.smarthome.core.library.types.DecimalType;
import org.eclipse.smarthome.core.library.types.OnOffType;
import org.eclipse.smarthome.core.library.types.OpenClosedType;
import org.eclipse.smarthome.core.library.types.StringType;
import org.eclipse.smarthome.core.thing.ChannelUID;
import org.eclipse.smarthome.core.thing.Thing;
import org.eclipse.smarthome.core.thing.ThingStatus;
import org.eclipse.smarthome.core.thing.binding.BaseThingHandler;
import org.eclipse.smarthome.core.types.Command;
import org.openhab.binding.sdds.SDDSTypes;
import org.openhab.binding.sdds.sddsBindingConstants;
import org.openhab.binding.sdds.discovery.SDDSDiscoveryService;
import org.openhab.binding.sdds.handler.SDDSLibrary.Alarm;
import org.openhab.binding.sdds.handler.SDDSLibrary.BatteryInformation;
import org.openhab.binding.sdds.handler.SDDSLibrary.ElectricityConsumptionSensor;
import org.openhab.binding.sdds.handler.SDDSLibrary.OccupancySensor;
import org.openhab.binding.sdds.handler.SDDSLibrary.OnOffLight;
import org.openhab.binding.sdds.handler.SDDSLibrary.PowerOutlet;
import org.openhab.binding.sdds.handler.SDDSLibrary.SingleInputContact;
import org.openhab.binding.sdds.handler.SDDSLibrary.SingleInputContactNonce;
//import java.util.logging.Logger;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * The {@link sddsHandler} is responsible for handling commands, which are
 * sent to one of the channels.
 *
 * @author David Diefenbach - Initial contribution
 * @author Paul Schmelzer - Extension
 * @author Tim Giesendorf - Extension
 */
public class sddsHandler extends BaseThingHandler implements SDDSListener {

    private Logger logger = LoggerFactory.getLogger(sddsHandler.class);
    private int last_nonce = 0;

    /**
     * @param thing
     */
    public sddsHandler(Thing thing) {
        super(thing);
    }

    /*
     * (non-Javadoc)
     *
     * @see org.eclipse.smarthome.core.thing.binding.BaseThingHandler#initialize()
     */
    @Override
    public void initialize() {
        super.initialize();
        SDDSDiscoveryService.sddsConn.register(getThing().getUID().getId(), this);
        updateStatus(ThingStatus.ONLINE);

    }

    /*
     * (non-Javadoc)
     *
     * @see org.eclipse.smarthome.core.thing.binding.BaseThingHandler#dispose()
     */
    @Override
    public void dispose() {
        SDDSDiscoveryService.sddsConn.unregister(getThing().getUID().getId());

        super.dispose();
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * org.eclipse.smarthome.core.thing.binding.ThingHandler#handleCommand(org.eclipse.smarthome.core.thing.ChannelUID,
     * org.eclipse.smarthome.core.types.Command)
     */
    @Override
    public void handleCommand(ChannelUID channelUID, Command command) {

        if (channelUID.getId().equals(sddsBindingConstants.OUTLETSTATE)
                || channelUID.getId().equals(sddsBindingConstants.LIGHTSTATE)
                || channelUID.getId().equals(sddsBindingConstants.ALARMTYPE)) {
            logger.debug("Command for SDDS Device received: " + channelUID.getId() + " " + command.toFullString());
            SDDSDiscoveryService.sddsConn.writeToSDDS(getThing().getUID().getId(), channelUID.getId(), command);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * org.openhab.binding.sdds.handler.SDDSContactListener#update(org.openhab.binding.sdds.handler.SDDSConnection.SDDS.
     * SingleInputContact)
     */
    @Override
    public void updateSingleInputContact(SingleInputContact input) {
        System.out.println("normaler contact");
        if (getThing().getStatus() == ThingStatus.OFFLINE) {
            updateStatus(ThingStatus.ONLINE);
        }
        if (input.contact > 0) {
            updateState(sddsBindingConstants.CONTACT, OpenClosedType.CLOSED);
        } else {
            updateState(sddsBindingConstants.CONTACT, OpenClosedType.OPEN);
        }

        updateState(sddsBindingConstants.OUTOFSERVICE, mapToOnOffType(input.outofservice));

    }

    /*
     * (non-Javadoc)
     *
     * @see org.openhab.binding.sdds.handler.SDDSContactListener#error()
     */
    @Override
    public void error() {
        updateStatus(ThingStatus.OFFLINE);

    }

    @Override
    public void updatePowerOutlet(PowerOutlet input) {
        if (getThing().getStatus() == ThingStatus.OFFLINE) {
            updateStatus(ThingStatus.ONLINE);
        }

        updateState(sddsBindingConstants.OUTLETSTATE, mapToOnOffType(input.state));

    }

    @Override
    public void updateOnOffLight(OnOffLight input) {
        if (getThing().getStatus() == ThingStatus.OFFLINE) {
            updateStatus(ThingStatus.ONLINE);
        }

        updateState(sddsBindingConstants.LIGHTSTATE, mapToOnOffType(input.state));

    }

    @Override
    public void updateElectricityConsumption(ElectricityConsumptionSensor input) {
        if (getThing().getStatus() == ThingStatus.OFFLINE) {
            updateStatus(ThingStatus.ONLINE);
        }

        updateState(sddsBindingConstants.MR, new DecimalType(input.mr));
        updateState(sddsBindingConstants.DT,
                StringType.valueOf(SDDSTypes.PowerCurrentCumType.values()[input.dt].value));
        updateState(sddsBindingConstants.DIV, StringType.valueOf(SDDSTypes.DivisorType.values()[input.div].value));

    }

    @Override
    public void updateOccupancy(OccupancySensor input) {
        if (getThing().getStatus() == ThingStatus.OFFLINE) {
            updateStatus(ThingStatus.ONLINE);
        }

        updateState(sddsBindingConstants.OCCUPANCY, mapToOccupancyType(input.occupancy));
        updateState(sddsBindingConstants.SENSORTYPE,
                StringType.valueOf(SDDSTypes.SensorType.values()[input.sensortype].value));
    }

    @Override
    public void updateAlarm(Alarm input) {
        if (getThing().getStatus() == ThingStatus.OFFLINE) {
            updateStatus(ThingStatus.ONLINE);
        }

        updateState(sddsBindingConstants.ALARM, mapToOnOffType(input.alarm));
        updateState(sddsBindingConstants.ALARMTYPE, StringType.valueOf(SDDSTypes.AlarmType.values()[input.type].value));
        updateState(sddsBindingConstants.SEVERITY,
                StringType.valueOf(SDDSTypes.SeverityType.values()[input.severity].value));
    }

    @Override
    public void updateBatteryInformation(BatteryInformation input) {
        if (getThing().getStatus() == ThingStatus.OFFLINE) {
            updateStatus(ThingStatus.ONLINE);
        }

    }

    private OnOffType mapToOnOffType(byte value) {
        if (value > 0) {
            return OnOffType.ON;
        } else {
            return OnOffType.OFF;
        }
    }

    private OnOffType mapToOccupancyType(byte value) {
        if (value > 0) {
            return OnOffType.OFF;
        } else {
            return OnOffType.ON;
        }
    }

    @Override
    public void updateSingleInputContactNonce(SingleInputContactNonce input) {
        // TODO Auto-generated method stub
        if (getThing().getStatus() == ThingStatus.OFFLINE) {
            updateStatus(ThingStatus.ONLINE);
        }
        if (input.contact > 0) {
            updateState(sddsBindingConstants.CONTACT, OpenClosedType.CLOSED);
        } else {
            updateState(sddsBindingConstants.CONTACT, OpenClosedType.OPEN);
        }

        int nonce = input.nonce;
        System.out.println("Nonce: " + nonce);
        long t = System.currentTimeMillis();
        boolean correct = false;
        int result = 0;
        t = t / 1000;
        t = t / 30;
        t = t & 0x0000FFFF;
        System.out.println("T: " + t);
        System.out.println("Last: " + last_nonce);
        if (t == nonce || t == nonce - 1 || t == nonce + 1) {
            if (last_nonce != nonce) {
                last_nonce = nonce;
                correct = true;
            }
        }
        if (correct == true) {
            result = 1;
        } else {
            result = 0;
        }

        updateState(sddsBindingConstants.OUTOFSERVICE, mapToOnOffType(input.outofservice));
        updateState(sddsBindingConstants.NONCE, new DecimalType(result));
        // updateState(sddsBindingConstants.NONCE, new DecimalType(input.nonce));
    }

}
