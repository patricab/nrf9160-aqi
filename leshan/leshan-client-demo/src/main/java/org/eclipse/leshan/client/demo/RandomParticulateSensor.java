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

public class RandomParticulateSensor extends BaseInstanceEnabler implements Destroyable {

    private static final Logger LOG = LoggerFactory.getLogger(RandomParticulateSensor.class);

    private static final String UNIT_MICROGRAM_PER_CUBIC_METER = "ug/m3";
    private static final float PARTICLE_SIZE_VALUE = 2.5f;
    private static final float MAX_RANGE_VALUE = 1000;
    private static final int SENSOR_VALUE = 5700;
    private static final int UNITS = 5701;
    private static final int MAX_MEASURED_VALUE = 5602;
    private static final int MIN_MEASURED_VALUE = 5601;
    private static final int MAX_RANGE_VALUE_ID = 5604;
    private static final int RESET_MIN_MAX_MEASURED_VALUES = 5605;
    private static final int TIMESTAMP = 5518;
    private static final int APPLICATION_TYPE = 5750;
    private static final int PARTICLE_SIZE = 6043;
    private static final List<Integer> supportedResources = Arrays.asList(SENSOR_VALUE, UNITS, MAX_MEASURED_VALUE,
            MIN_MEASURED_VALUE, MAX_RANGE_VALUE_ID, RESET_MIN_MAX_MEASURED_VALUES, TIMESTAMP, APPLICATION_TYPE, PARTICLE_SIZE);
    private final ScheduledExecutorService scheduler;
    private final Random rng = new Random();
    private Date timestamp;
    private String application_type;
    private double currentParticulate = 100d;
    private double minMeasuredValue = currentParticulate;
    private double maxMeasuredValue = currentParticulate;

    public RandomParticulateSensor() {
        this.scheduler = Executors.newSingleThreadScheduledExecutor(new NamedThreadFactory("Particulate Sensor"));
        scheduler.scheduleAtFixedRate(new Runnable() {

            @Override
            public void run() {
                adjustParticulate();
            }
        }, 2, 30, TimeUnit.SECONDS);
    }

    @Override
    public synchronized ReadResponse read(ServerIdentity identity, int resourceId) {
        LOG.info("Read on Particulate resource /{}/{}/{}", getModel().id, getId(), resourceId);
        switch (resourceId) {
        case MIN_MEASURED_VALUE:
            return ReadResponse.success(resourceId, getTwoDigitValue(minMeasuredValue));
        case MAX_MEASURED_VALUE:
            return ReadResponse.success(resourceId, getTwoDigitValue(maxMeasuredValue));
        case MAX_RANGE_VALUE_ID:
            return ReadResponse.success(resourceId, MAX_RANGE_VALUE);
        case SENSOR_VALUE:
            return ReadResponse.success(resourceId, getTwoDigitValue(currentParticulate));
        case UNITS:
            return ReadResponse.success(resourceId, UNIT_MICROGRAM_PER_CUBIC_METER);
        case TIMESTAMP:
            return ReadResponse.success(resourceId, getTimestamp());
        case APPLICATION_TYPE:
            return ReadResponse.success(resourceId, getApplicationType());
        case PARTICLE_SIZE:
            return ReadResponse.success(resourceId, PARTICLE_SIZE_VALUE);
        default:
            return super.read(identity, resourceId);
        }
    }

    @Override
    public synchronized ExecuteResponse execute(ServerIdentity identity, int resourceId, String params) {
        LOG.info("Execute on Particulate resource /{}/{}/{}", getModel().id, getId(), resourceId);
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

    private void adjustParticulate() {
        float delta = (rng.nextInt(50) - 1) / 10f;
        currentParticulate += delta;
        Integer changedResource = adjustMinMaxMeasuredValue(currentParticulate);
        timestamp = new Date();
        if (changedResource != null) {
            fireResourcesChange(SENSOR_VALUE, changedResource);
        } else {
            fireResourcesChange(SENSOR_VALUE);
        }
    }

    private synchronized Integer adjustMinMaxMeasuredValue(double newParticulate) {
        if (newParticulate > maxMeasuredValue) {
            maxMeasuredValue = newParticulate;
            return MAX_MEASURED_VALUE;
        } else if (newParticulate < minMeasuredValue) {
            minMeasuredValue = newParticulate;
            return MIN_MEASURED_VALUE;
        } else {
            return null;
        }
    }

    private void resetMinMaxMeasuredValues() {
        minMeasuredValue = currentParticulate;
        maxMeasuredValue = currentParticulate;
    }

    private Date getTimestamp() {
        return timestamp;
    }

    private String getApplicationType() {
        return application_type;
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
