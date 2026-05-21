#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
ARTIFACTS_DIR="${BUILD_DIR}/RSaturator_artefacts"
PACKAGE_DIR="${BUILD_DIR}/packages"
PAYLOAD_DIR="${BUILD_DIR}/pkg-payload"
IDENTIFIER="com.ranze.rsaturator.pkg"
VERSION="${RSATURATOR_VERSION:-0.2.0}"

PKG_OUTPUT="${PACKAGE_DIR}/R-Saturator-${VERSION}-macOS.pkg"

VST3_SOURCE="$(find "${ARTIFACTS_DIR}" -type d -name "R-Saturator.vst3" | head -n 1)"
AU_SOURCE="$(find "${ARTIFACTS_DIR}" -type d -name "R-Saturator.component" | head -n 1)"

if [[ ! -d "${VST3_SOURCE}" ]]; then
    echo "Missing VST3 bundle under: ${ARTIFACTS_DIR}" >&2
    echo "Run: cmake --build build --config Release" >&2
    exit 1
fi

if [[ ! -d "${AU_SOURCE}" ]]; then
    echo "Missing AU bundle under: ${ARTIFACTS_DIR}" >&2
    echo "Run: cmake --build build --config Release" >&2
    exit 1
fi

rm -rf "${PAYLOAD_DIR}" "${PACKAGE_DIR}"
mkdir -p "${PAYLOAD_DIR}/Library/Audio/Plug-Ins/VST3"
mkdir -p "${PAYLOAD_DIR}/Library/Audio/Plug-Ins/Components"
mkdir -p "${PACKAGE_DIR}"

ditto "${VST3_SOURCE}" "${PAYLOAD_DIR}/Library/Audio/Plug-Ins/VST3/R-Saturator.vst3"
ditto "${AU_SOURCE}" "${PAYLOAD_DIR}/Library/Audio/Plug-Ins/Components/R-Saturator.component"

pkgbuild \
    --root "${PAYLOAD_DIR}" \
    --identifier "${IDENTIFIER}" \
    --version "${VERSION}" \
    --install-location "/" \
    "${PKG_OUTPUT}"

echo "Created ${PKG_OUTPUT}"
