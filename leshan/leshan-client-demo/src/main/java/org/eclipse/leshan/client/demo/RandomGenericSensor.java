package org.eclipse.leshan.client.demo;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.Arrays;
import java.util.Date;
import java.util.List;
import java.util.Random;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import org.eclipse.leshan.client.resource.BaseInstanceEnabler;
import org.eclipse.leshan.client.servers.ServerIdentity;
import org.eclipse.leshan.core.Destroyable;
import org.eclipse.leshan.core.model.ObjectModel;
import org.eclipse.leshan.core.response.ExecuteResponse;
import org.eclipse.leshan.core.response.ReadResponse;
import org.eclipse.leshan.core.util.NamedThreadFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class RandomGenericSensor extends BaseInstanceEnabler implements Destroyable {

    private static final Logger LOG = LoggerFactory.getLogger(RandomGenericSensor.class);

    private static final String UNIT_CONC = "ug/m3";
    private static final int SENSOR_VALUE = 5700;
    private static final int UNITS = 5701;
    private static final int MIN_MEASURED_VALUE = 5601;
    private static final int MAX_MEASURED_VALUE = 5602;
    private static final int RESET_MIN_MAX_MEASURED_VALUES = 5605;
    private static final int TIMESTAMP = 5518;
    private static final List<Integer> supportedResources = Arrays.asList(SENSOR_VALUE, UNITS, MAX_MEASURED_VALUE,
            MIN_MEASURED_VALUE, RESET_MIN_MAX_MEASURED_VALUES, TIMESTAMP);
    private final ScheduledExecutorService scheduler;
    private final Random rng = new Random();
    private Date timestamp;
    private double currentGen = 20d;
    private double minMeasuredValue = currentGen;
    private double maxMeasuredValue = currentGen;

    public RandomGenericSensor() {
        this.scheduler = Executors.newSingleThreadScheduledExecutor(new NamedThreadFactory("Generic Sensor"));
        scheduler.scheduleAtFixedRate(new Runnable() {

            @Override
            public void run() {
                adjustGeneric();
            }
        }, 2, 30, TimeUnit.SECONDS);
    }

    @Override
    public synchronized ReadResponse read(ServerIdentity identity, int resourceId) {
        LOG.info("Read on Generic resource /{}/{}/{}", getModel().id, getId(), resourceId);
        switch (resourceId) {
        case MIN_MEASURED_VALUE:
            return ReadResponse.success(resourceId, getTwoDigitValue(minMeasuredValue));
        case MAX_MEASURED_VALUE:
            return ReadResponse.success(resourceId, getTwoDigitValue(maxMeasuredValue));
        case SENSOR_VALUE:
            return ReadResponse.success(resourceId, getTwoDigitValue(currentGen));
        case UNITS:
            return ReadResponse.success(resourceId, UNIT_CONC);
        case TIMESTAMP:
            return ReadResponse.success(resourceId, getTimestamp());
        default:
            return super.read(identity, resourceId);
        }
    }

    @Override
    public synchronized ExecuteResponse execute(ServerIdentity identity, int resourceId, String params) {
        LOG.info("Execute on Generic resource /{}/{}/{}", getModel().id, getId(), resourceId);
        switch (resourceId) {
        case RESET_MIN_MAX_MEASURED_VALUES:
            resetMinMaxMeasuredValues();
            return ExecuteResponse.success();
        default:
            return super.execute(identity, resourceId, params);
        }
    }

    private double getTwoDigitValue(double value) {
        BigDecimal toBeTruncated = BigDecimal.valueOf(value);
        return toBeTruncated.setScale(2, RoundingMode.HALF_UP).doubleValue();
    }

    private void adjustGeneric() {
        float delta = (rng.nextInt(20) - 10) / 10f;
        currentGen += delta;
        Integer changedResource = adjustMinMaxMeasuredValue(currentGen);
        timestamp = new Date();
        if (changedResource != null) {
            fireResourcesChange(SENSOR_VALUE, changedResource);
        } else {
            fireResourcesChange(SENSOR_VALUE);
        }
    }

    private synchronized Integer adjustMinMaxMeasuredValue(double newGeneric) {
        if (newGeneric > maxMeasuredValue) {
            maxMeasuredValue = newGeneric;
            return MAX_MEASURED_VALUE;
        } else if (newGeneric < minMeasuredValue) {
            minMeasuredValue = newGeneric;
            return MIN_MEASURED_VALUE;
        } else {
            return null;
        }
    }

    private void resetMinMaxMeasuredValues() {
        minMeasuredValue = currentGen;
        maxMeasuredValue = currentGen;
    }

    private Date getTimestamp() {
        return timestamp;
    }

    @Override
    public List<Integer> getAvailableResourceIds(ObjectModel model) {
        return supportedResources;
    }

    @Override
    public void destroy() {
        scheduler.shutdown();
    }
}
